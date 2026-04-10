#pragma once

#include <array>
#include <optional>
#include <random>
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include "Ant.h"
#include "Id.h"
#include "Pheromone.h"
#include "Position.h"
#include "Tile.h"

/**
 * @brief The main world class managing all tiles and coordinates
 */
class World {
private:
    std::vector<Tile> tiles;
    std::vector<std::array<float, kPheromoneTypeCount>> pheromoneScratch;
    std::vector<std::shared_ptr<Ant>> ants;
    std::unique_ptr<IntegerPosition> nestEntrancePosition;
    std::mt19937 rng;
    UniqueIdGenerator idGenerator;

    std::size_t tileIndex(int x, int y) const { return static_cast<std::size_t>(y) * width + x; }

    void updateAnts();
    std::shared_ptr<Ant> createAnt(AntRole role, const IntegerPosition& pos);

public:
    const unsigned int width;
    const unsigned int height;
    World(unsigned int width, unsigned int height, unsigned int initial_colony_size,
          std::optional<unsigned int> seed = std::nullopt);

    // World initialization
    void initialize(unsigned int initial_colony_size);
    void generateTerrain();
    void placeNest(const IntegerPosition& pos);
    void placeFood(const IntegerPosition& pos, float amount);
    
    // Tile access
    Tile* getTile(const IntegerPosition& pos);
    Tile* getTile(const FloatPosition& pos);
    Tile* getTile(int x, int y);
    bool isValidPosition(const IntegerPosition& pos) const;
    bool isValidPosition(const FloatPosition& pos) const;
    bool isValidPosition(int x, int y) const;
    float distanceToNest(const FloatPosition& pos) const;
    
    // Pathfinding
    std::vector<IntegerPosition> getAdjacentPositions(const IntegerPosition& pos);
    
    // World interactions
    void depositPheromone(const IntegerPosition& pos, PheromoneType type, float amount);
    void updatePheromones();
    void spawnFood(int count);
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