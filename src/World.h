#pragma once

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include "Ant.h"
#include "Position.h"
#include "Tile.h"

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
 * @brief The main world class managing all tiles and coordinates
 */
class World {
private:
    std::unordered_map<IntegerPosition, std::unique_ptr<Tile>> tiles;
    std::vector<std::shared_ptr<Ant>> ants;
    std::unique_ptr<IntegerPosition> nestEntrancePosition;

    void updateAnts();
    
public:
    const unsigned int width;
    const unsigned int height;
    World(unsigned int width, unsigned int height, unsigned int initial_colony_size);
    
    // World initialization
    void initialize(unsigned int initial_colony_size);
    void generateTerrain();
    void placeNest(const IntegerPosition& pos);
    void placeFood(const IntegerPosition& pos, float amount);
    
    // Tile access
    Tile* getTile(const IntegerPosition& pos);
    Tile* getTile(int x, int y);
    bool isValidPosition(const IntegerPosition& pos) const;
    bool isValidPosition(const FloatPosition& pos) const;
    bool isValidPosition(int x, int y) const;
    float distanceToNest(const FloatPosition& pos) const;
    
    // Pathfinding
    std::vector<IntegerPosition> getAdjacentPositions(const IntegerPosition& pos);
    
    // World interactions
    void addPheromone(const IntegerPosition& pos, const std::string& message, int strength = 10);
    void updatePheromones();
    void spawnFood(int count, float amountPerSpawn);
    void update();
    
    // Ant management
    void addAnt(std::shared_ptr<Ant> ant, const IntegerPosition& pos);
    IntegerPosition getNestEntrancePosition() const;
    
    // World properties
    int getWidth() const;
    int getHeight() const;
    const std::vector<std::shared_ptr<Ant>>& getAnts() const;
    
    // Iteration over tiles
    void forEachTile(std::function<void(Tile*)> callback);
};