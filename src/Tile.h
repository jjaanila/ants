#pragma once

#include <vector>
#include <string>
#include "Ant.h"
#include "Position.h"

/**
 * @brief Types of terrain that can exist in a tile
 */
enum class TerrainType {
    SOIL,
    SAND,
    ROCK,
    GRASS,
};

/**
 * @brief Represents a single tile in the world
 */
class Tile {
private:
    IntegerPosition position;
    TerrainType terrain;
    bool hasFood;
    float foodAmount;
    bool hasPheromone;
    std::string pheromoneMessage;
    int pheromoneStrength;
    bool isNestEntrance;
    std::vector<std::shared_ptr<Ant>> ants;
    
public:
    Tile(IntegerPosition pos, TerrainType terrain = TerrainType::SOIL);
    
    // Getters
    IntegerPosition getPosition() const;
    TerrainType getTerrain() const;
    bool getHasFood() const;
    float getFoodAmount() const;
    bool getHasPheromone() const;
    std::string getPheromoneMessage() const;
    int getPheromoneStrength() const;
    bool getIsNestEntrance() const;
    const std::vector<std::shared_ptr<Ant>>& getAnts() const;
    
    // Setters
    void setTerrain(TerrainType newTerrain);
    void addFood(float amount);
    void removeFood(float amount);
    void addPheromone(const std::string& message, int strength = 10);
    void decreasePheromoneStrength(int amount = 1);
    void setNestEntrance(bool isEntrance);
    
    // Ant management
    void addAnt(std::shared_ptr<Ant> ant);
    void removeAnt(std::shared_ptr<Ant> ant);
    bool hasAnts() const;
    int getAntCount() const;
    std::string getDescription() const;
};