#include <iostream>
#include <random>

#include "Tile.h"

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
    return ants;
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
    ants.push_back(ant);
}

void Tile::removeAnt(std::shared_ptr<Ant> ant) {
    auto it = std::find(ants.begin(), ants.end(), ant);
    if (it != ants.end()) {
        ants.erase(it);
    }
}

bool Tile::hasAnts() const {
    return !ants.empty();
}

int Tile::getAntCount() const {
    return ants.size();
}

std::string Tile::getDescription() const {
    std::string desc = "Tile at " + position.toString() + " - ";
    
    // Terrain type
    switch (terrain) {
        case TerrainType::SOIL: desc += "Soil"; break;
        case TerrainType::SAND: desc += "Sand"; break;
        case TerrainType::ROCK: desc += "Rock"; break;
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
        desc += ", Ants: " + std::to_string(ants.size());
    }
    
    return desc;
}