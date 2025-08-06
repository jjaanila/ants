#include "World.h"
#include <iostream>
#include <random>
#include <queue>
#include <unordered_set>

// Tile implementation
Tile::Tile(IntegerPosition pos, TerrainType terrain) 
    : position(pos), 
      terrain(terrain), 
      hasFood(false), 
      foodAmount(0.0), 
      hasPheromone(false), 
      pheromoneStrength(0), 
      isNestEntrance(false) {
}

IntegerPosition Tile::getPosition() const {
    return position;
}

TerrainType Tile::getTerrain() const {
    return terrain;
}

bool Tile::getHasFood() const {
    return hasFood;
}

float Tile::getFoodAmount() const {
    return foodAmount;
}

bool Tile::getHasPheromone() const {
    return hasPheromone;
}

std::string Tile::getPheromoneMessage() const {
    return pheromoneMessage;
}

int Tile::getPheromoneStrength() const {
    return pheromoneStrength;
}

bool Tile::getIsNestEntrance() const {
    return isNestEntrance;
}

const std::vector<std::shared_ptr<Ant>>& Tile::getAnts() const {
    return antsOnTile;
}

void Tile::setTerrain(TerrainType newTerrain) {
    terrain = newTerrain;
}

void Tile::addFood(float amount) {
    foodAmount += amount;
    hasFood = foodAmount > 0;
}

void Tile::removeFood(float amount) {
    foodAmount -= amount;
    if (foodAmount <= 0) {
        foodAmount = 0;
        hasFood = false;
    }
}

void Tile::addPheromone(const std::string& message, int strength) {
    pheromoneMessage = message;
    pheromoneStrength = strength;
    hasPheromone = true;
}

void Tile::decreasePheromoneStrength(int amount) {
    pheromoneStrength -= amount;
    if (pheromoneStrength <= 0) {
        pheromoneStrength = 0;
        hasPheromone = false;
    }
}

void Tile::setNestEntrance(bool isEntrance) {
    isNestEntrance = isEntrance;
}

void Tile::addAnt(std::shared_ptr<Ant> ant) {
    antsOnTile.push_back(ant);
}

void Tile::removeAnt(std::shared_ptr<Ant> ant) {
    auto it = std::find(antsOnTile.begin(), antsOnTile.end(), ant);
    if (it != antsOnTile.end()) {
        antsOnTile.erase(it);
    }
}

bool Tile::hasAnts() const {
    return !antsOnTile.empty();
}

int Tile::getAntCount() const {
    return antsOnTile.size();
}

std::string Tile::getDescription() const {
    std::string desc = "Tile at " + position.toString() + " - ";
    
    // Terrain type
    switch (terrain) {
        case TerrainType::SOIL: desc += "Soil"; break;
        case TerrainType::SAND: desc += "Sand"; break;
        case TerrainType::ROCK: desc += "Rock"; break;
        case TerrainType::WATER: desc += "Water"; break;
        case TerrainType::GRASS: desc += "Grass"; break;
    }
    
    // Additional information
    if (isNestEntrance) {
        desc += " (Nest Entrance)";
    }
    
    if (hasFood) {
        desc += ", Food: " + std::to_string(foodAmount);
    }
    
    if (hasPheromone) {
        desc += ", Pheromone: \"" + pheromoneMessage + "\" (Strength: " 
               + std::to_string(pheromoneStrength) + ")";
    }
    
    if (hasAnts()) {
        desc += ", Ants: " + std::to_string(antsOnTile.size());
    }
    
    return desc;
}

// World implementation
World::World(unsigned int width, unsigned int height) : width(width), height(height) {
    initialize();
}

void World::initialize() {
    // Create all tiles
    for (unsigned int x = 0; x < width; x++) {
        for (unsigned int y = 0; y < height; y++) {
            IntegerPosition pos(x, y);
            tiles[pos] = std::make_unique<Tile>(pos, TerrainType::SOIL);
        }
    }
    
    // Generate interesting terrain
    generateTerrain();
    
    // Place nest at the center by default
    placeNest(IntegerPosition(width / 2, height / 2));
    spawnFood(50, 5.0);
}

IntegerPosition World::getNestEntrancePosition() const {
    return *nestEntrancePosition;
}

void World::placeNest(const IntegerPosition& pos) {
    if (isValidPosition(pos)) {
        nestEntrancePosition = std::make_unique<IntegerPosition>(pos);
        getTile(pos)->setNestEntrance(true);
        
        // Make adjacent tiles soil for easier access
        for (auto& adjPos : getAdjacentPositions(pos)) {
            getTile(adjPos)->setTerrain(TerrainType::SOIL);
        }
    }
}

int World::getWidth() const {
    return width;
}

int World::getHeight() const {
    return height;
}

void World::generateTerrain() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, 100);
    std::uniform_int_distribution<> terrainType(0, 4);
    
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            // 10% chance of special terrain
            if (distr(gen) < 10) {
                TerrainType terrain = static_cast<TerrainType>(terrainType(gen));
                getTile(x, y)->setTerrain(terrain);
            }
        }
    }
    
    // Make some water bodies
    int numWaterBodies = width * height / 200;
    for (int i = 0; i < numWaterBodies; i++) {
        int centerX = distr(gen) % width;
        int centerY = distr(gen) % height;
        
        int size = 3 + distr(gen) % 5;
        for (int dx = -size; dx <= size; dx++) {
            for (int dy = -size; dy <= size; dy++) {
                int distance = dx*dx + dy*dy;
                if (distance <= size*size && isValidPosition(centerX + dx, centerY + dy)) {
                    if (distr(gen) < 70) {
                        getTile(centerX + dx, centerY + dy)->setTerrain(TerrainType::WATER);
                    }
                }
            }
        }
    }
}

float World::distanceToNest(const FloatPosition& pos) const {
    return pos.distanceTo(getNestEntrancePosition());
}

void World::placeFood(const IntegerPosition& pos, float amount) {
    if (isValidPosition(pos)) {
        // Don't place food on water or nest
        Tile* tile = getTile(pos);
        if (tile->getTerrain() != TerrainType::WATER && !tile->getIsNestEntrance()) {
            tile->addFood(amount);
        }
    }
}

Tile* World::getTile(const IntegerPosition& pos) {
    if (isValidPosition(pos)) {
        return tiles[pos].get();
    }
    return nullptr;
}

Tile* World::getTile(int x, int y) {
    return getTile(IntegerPosition(x, y));
}

bool World::isValidPosition(const IntegerPosition& pos) const {
    return pos.getX() >= 0 && pos.getX() < width && pos.getY() >= 0 && pos.getY() < height;
}

bool World::isValidPosition(int x, int y) const {
    return isValidPosition(IntegerPosition(x, y));
}

std::vector<IntegerPosition> World::getAdjacentPositions(const IntegerPosition& pos) {
    std::vector<IntegerPosition> adjacentPositions;
    
    // 4-way adjacency (North, East, South, West)
    const std::vector<std::pair<int, int>> directions = {
        {0, -1}, {1, 0}, {0, 1}, {-1, 0}
    };
    
    for (const auto& dir : directions) {
        IntegerPosition newPosition(pos.getX() + dir.first, pos.getY() + dir.second);
        if (isValidPosition(newPosition)) {
            adjacentPositions.push_back(newPosition);
        }
    }
    
    return adjacentPositions;
}

void World::addPheromone(const IntegerPosition& pos, const std::string& message, int strength) {
    if (isValidPosition(pos)) {
        getTile(pos)->addPheromone(message, strength);
    }
}

void World::updatePheromones() {
    forEachTile([](Tile* tile) {
        if (tile->getHasPheromone()) {
            tile->decreasePheromoneStrength();
        }
    });
}

void World::spawnFood(int count, float amountPerSpawn) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> posX(0, width - 1);
    std::uniform_int_distribution<> posY(0, height - 1);
    
    for (int i = 0; i < count; i++) {
        // Try to find suitable location
        for (int attempts = 0; attempts < 10; attempts++) {
            IntegerPosition pos(posX(gen), posY(gen));
            Tile* tile = getTile(pos);
            
            if (tile && 
                tile->getTerrain() != TerrainType::WATER && 
                !tile->getIsNestEntrance() &&
                !tile->getHasFood()) {
                
                placeFood(pos, amountPerSpawn);
                break;
            }
        }
    }
}

void World::addAnt(std::shared_ptr<Ant> ant, const IntegerPosition& pos) {
    if (isValidPosition(pos)) {
        getTile(pos)->addAnt(ant);
        (*ant).setPosition(FloatPosition(pos));
    }
}

void World::forEachTile(std::function<void(Tile*)> callback) {
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            IntegerPosition pos(x, y);
            callback(getTile(pos));
        }
    }
}