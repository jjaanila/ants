#pragma once

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include "Ant.h"
#include "Position.h"

// Forward declarations
class Tile;

// Custom hash function for Position to use in unordered_map
namespace std {
    template <>
    struct hash<IntegerPosition> {
        std::size_t operator()(const IntegerPosition& pos) const {
            // Simple hash combining both coordinates
            return std::hash<int>()(pos.getX()) ^ (std::hash<int>()(pos.getY()) << 1);
        }
    };
}

/**
 * @brief Types of terrain that can exist in a tile
 */
enum class TerrainType {
    SOIL,
    SAND,
    ROCK,
    WATER,
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
    std::vector<std::shared_ptr<Ant>> antsOnTile;
    
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

/**
 * @brief The main world class managing all tiles and coordinates
 */
class World {
private:
    std::unordered_map<IntegerPosition, std::unique_ptr<Tile>> tiles;
    std::unique_ptr<IntegerPosition> nestEntrancePosition;
    
public:
    const unsigned int width;
    const unsigned int height;
    World(unsigned int width, unsigned int height);
    
    // World initialization
    void initialize();
    void generateTerrain();
    void placeNest(const IntegerPosition& pos);
    void placeFood(const IntegerPosition& pos, float amount);
    
    // Tile access
    Tile* getTile(const IntegerPosition& pos);
    Tile* getTile(int x, int y);
    bool isValidPosition(const IntegerPosition& pos) const;
    bool isValidPosition(int x, int y) const;
    float distanceToNest(const FloatPosition& pos) const;
    
    // Pathfinding
    std::vector<IntegerPosition> getAdjacentPositions(const IntegerPosition& pos);
    
    // World interactions
    void addPheromone(const IntegerPosition& pos, const std::string& message, int strength = 10);
    void updatePheromones();
    void spawnFood(int count, float amountPerSpawn);
    
    // Ant management
    void addAnt(std::shared_ptr<Ant> ant, const IntegerPosition& pos);
    IntegerPosition getNestEntrancePosition() const;
    
    // World properties
    int getWidth() const;
    int getHeight() const;
    
    // Iteration over tiles
    void forEachTile(std::function<void(Tile*)> callback);
    
    // Display
    void display() const;
};