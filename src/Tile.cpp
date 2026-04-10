#include <algorithm>

#include "Tile.h"

Tile::Tile(IntegerPosition pos, TerrainType terrain)
    : position(pos),
      terrain(terrain),
      hasFood(false),
      foodAmount(0.0),
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

float Tile::getPheromone(PheromoneType type) const {
    return pheromones[static_cast<std::size_t>(type)];
}

bool Tile::hasAnyPheromone() const {
    for (float value : pheromones) {
        if (value > 0.0f) return true;
    }
    return false;
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

void Tile::depositPheromone(PheromoneType type, float amount) {
    pheromones[static_cast<std::size_t>(type)] += amount;
}

void Tile::setPheromone(PheromoneType type, float value) {
    pheromones[static_cast<std::size_t>(type)] = value;
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

    switch (terrain) {
        case TerrainType::SOIL: desc += "Soil"; break;
        case TerrainType::SAND: desc += "Sand"; break;
        case TerrainType::ROCK: desc += "Rock"; break;
        case TerrainType::GRASS: desc += "Grass"; break;
    }

    if (isNestEntrance) {
        desc += " (Nest Entrance)";
    }

    if (hasFood) {
        desc += ", Food: " + std::to_string(foodAmount);
    }

    if (hasAnyPheromone()) {
        desc += ", FoodTrail: " + std::to_string(getPheromone(PheromoneType::FoodTrail));
    }

    if (hasAnts()) {
        desc += ", Ants: " + std::to_string(ants.size());
    }

    return desc;
}
