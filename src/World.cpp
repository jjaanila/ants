#include <iostream>
#include <random>
#include <queue>
#include <unordered_set>
#include "rng.h"
#include "Tile.h"
#include "World.h"


// World implementation
World::World(unsigned int width, unsigned int height, const unsigned int initial_colony_size)
    :
    width(width),
    height(height) {
    initialize(initial_colony_size);
}

void World::initialize(const unsigned int initial_colony_size) {
    for (unsigned int x = 0; x < width; x++) {
        for (unsigned int y = 0; y < height; y++) {
            IntegerPosition pos(x, y);
            tiles[pos] = std::make_unique<Tile>(pos, TerrainType::SOIL);
        }
    }
    
    generateTerrain();
    
    placeNest(IntegerPosition(width / 2, height / 2));
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    const IntegerPosition nestPosition = getNestEntrancePosition();
    addAnt(std::make_shared<Ant>(AntRole::QUEEN), nestPosition);
    for (int i = 1; i < initial_colony_size; ++i) {
        int roleRandom = getRandomInt(1, 100);
        AntRole role;
        
        if (roleRandom <= 45) {
            role = AntRole::WORKER;
        } else if (roleRandom <= 70) {
            role = AntRole::FORAGER;
        } else if (roleRandom <= 85) {
            role = AntRole::SOLDIER;
        } else if (roleRandom <= 95) {
            role = AntRole::NURSE;
        } else {
            role = AntRole::DRONE;
        }
        addAnt(std::make_shared<Ant>(role), nestPosition);
    }
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
}

float World::distanceToNest(const FloatPosition& pos) const {
    return pos.distanceTo(getNestEntrancePosition());
}

void World::placeFood(const IntegerPosition& pos, float amount) {
    if (isValidPosition(pos)) {
        Tile* tile = getTile(pos);
        if (!tile->getIsNestEntrance()) {
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

const std::vector<std::shared_ptr<Ant>>& World::getAnts() const {
    return ants;
}

bool World::isValidPosition(const IntegerPosition& pos) const {
    return pos.getX() >= 0 && pos.getX() < width && pos.getY() >= 0 && pos.getY() < height;
}

bool World::isValidPosition(const FloatPosition& pos) const {
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

void World::updateAnts() {
    for (auto& ant : ants) {
        (*ant).update(*this);
    }
}

void World::update() {
    updateAnts();
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
        ants.push_back(ant);
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