#include <random>
#include "Tile.h"
#include "World.h"


World::World(unsigned int width, unsigned int height, const unsigned int initial_colony_size,
             std::optional<unsigned int> seed)
    :
    pheromoneScratch(static_cast<std::size_t>(width) * height),
    rng(seed.value_or(std::random_device{}())),
    width(width),
    height(height) {
    tiles.reserve(static_cast<std::size_t>(width) * height);
    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; ++x) {
            tiles.emplace_back(IntegerPosition(x, y), TerrainType::SOIL);
        }
    }
    initialize(initial_colony_size);
}

std::shared_ptr<Ant> World::createAnt(AntRole role, const IntegerPosition& pos) {
    auto ant = std::make_shared<Ant>(role, idGenerator.getNextId(), rng);
    addAnt(ant, pos);
    return ant;
}

void World::initialize(const unsigned int initial_colony_size) {
    generateTerrain();

    placeNest(IntegerPosition(width / 2, height / 2));
    const IntegerPosition nestPosition = getNestEntrancePosition();
    createAnt(AntRole::QUEEN, nestPosition);

    std::uniform_int_distribution<int> roleRoll(1, 100);
    for (int i = 1; i < initial_colony_size; ++i) {
        const int roll = roleRoll(rng);
        AntRole role;
        if (roll <= 45) {
            role = AntRole::WORKER;
        } else if (roll <= 70) {
            role = AntRole::FORAGER;
        } else if (roll <= 85) {
            role = AntRole::SOLDIER;
        } else if (roll <= 95) {
            role = AntRole::NURSE;
        } else {
            role = AntRole::DRONE;
        }
        createAnt(role, nestPosition);
    }
    spawnFood(width * height / 20);
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
    std::uniform_int_distribution<> distr(0, 100);
    std::uniform_int_distribution<> terrainType(0, 4);

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            // 10% chance of special terrain
            if (distr(rng) < 10) {
                TerrainType terrain = static_cast<TerrainType>(terrainType(rng));
                getTile(x, y)->setTerrain(terrain);
            }
        }
    }
}

float World::distanceToNest(const FloatPosition& pos) const {
    return pos.distanceTo(getNestEntrancePosition());
}

Tile* World::getTile(int x, int y) {
    if (!isValidPosition(x, y)) return nullptr;
    return &tiles[tileIndex(x, y)];
}

Tile* World::getTile(const IntegerPosition& pos) {
    return getTile(static_cast<int>(pos.getX()), static_cast<int>(pos.getY()));
}

Tile* World::getTile(const FloatPosition& pos) {
    return getTile(pos.toIntegerPosition());
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

void World::depositPheromone(const IntegerPosition& pos, PheromoneType type, float amount) {
    if (isValidPosition(pos)) {
        getTile(pos)->depositPheromone(type, amount);
    }
}

void World::updatePheromones() {
    // Double-buffered diffusion + decay. For each tile, the next value is a
    // blend of the tile's current value and the average of its 4-neighbours,
    // then multiplied by a decay factor. Values below a floor snap to zero so
    // faint trails don't linger indefinitely. The scratch buffer lives on the
    // World so no per-tick allocation happens.
    static constexpr float kSelfWeight = 0.80f;
    static constexpr float kNeighborWeight = 1.0f - kSelfWeight;
    static constexpr float kDecay = 0.95f;
    static constexpr float kFloor = 0.05f;

    const int w = static_cast<int>(width);
    const int h = static_cast<int>(height);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            const std::size_t idx = tileIndex(x, y);
            for (std::size_t t = 0; t < kPheromoneTypeCount; ++t) {
                const auto type = static_cast<PheromoneType>(t);
                const float self = tiles[idx].getPheromone(type);

                float neighborSum = 0.0f;
                int neighborCount = 0;
                if (x > 0)     { neighborSum += tiles[tileIndex(x - 1, y)].getPheromone(type); ++neighborCount; }
                if (x < w - 1) { neighborSum += tiles[tileIndex(x + 1, y)].getPheromone(type); ++neighborCount; }
                if (y > 0)     { neighborSum += tiles[tileIndex(x, y - 1)].getPheromone(type); ++neighborCount; }
                if (y < h - 1) { neighborSum += tiles[tileIndex(x, y + 1)].getPheromone(type); ++neighborCount; }
                const float neighborAvg = neighborCount > 0 ? neighborSum / neighborCount : 0.0f;

                float blended = (self * kSelfWeight + neighborAvg * kNeighborWeight) * kDecay;
                if (blended < kFloor) blended = 0.0f;
                pheromoneScratch[idx][t] = blended;
            }
        }
    }

    for (std::size_t i = 0; i < tiles.size(); ++i) {
        for (std::size_t t = 0; t < kPheromoneTypeCount; ++t) {
            tiles[i].setPheromone(static_cast<PheromoneType>(t), pheromoneScratch[i][t]);
        }
    }
}

void World::updateAnts() {
    for (auto& ant : ants) {
        (*ant).update(*this);
    }
}

void World::update() {
    updateAnts();
    updatePheromones();
}

void World::placeFood(const IntegerPosition& pos, float amount) {
    if (isValidPosition(pos)) {
        Tile* tile = getTile(pos);
        if (!tile->getIsNestEntrance()) {
            tile->addFood(amount);
        }
    }
}

void World::spawnFood(int count) {
    std::uniform_int_distribution<> posX(0, width - 1);
    std::uniform_int_distribution<> posY(0, height - 1);
    std::uniform_int_distribution<> amount(1, 100);

    for (int i = 0; i < count; i++) {
        // Try to find suitable location
        for (int attempts = 0; attempts < 10; attempts++) {
            IntegerPosition pos(posX(rng), posY(rng));
            Tile* tile = getTile(pos);

            if (tile &&
                !tile->getIsNestEntrance() &&
                !tile->getHasFood()) {

                placeFood(pos, amount(rng));
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
    for (Tile& tile : tiles) {
        callback(&tile);
    }
}