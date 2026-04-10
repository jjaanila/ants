#pragma once

#include <memory>
#include <optional>
#include <random>
#include <unordered_map>

#include <SFML/Graphics/Color.hpp>
#include "Position.h"

class MovementStrategy;
class World;

enum class ItemType {
    FOOD,
    LARVA,
    EGG,
};

enum class AntRole {
    QUEEN,
    WORKER,
    SOLDIER,
    DRONE,
    FORAGER,
    NURSE
};

class Ant {
private:
    std::mt19937& rng;
    int id;
    AntRole role;
    float size;
    sf::Color color;
    float movementSpeed;
    float maxLoad;
    std::unique_ptr<FloatPosition> position;
    std::unique_ptr<FloatPosition> previousPosition;
    std::unique_ptr<MovementStrategy> movementStrategy;
    Vector2D lastDirection;
    float initialWanderRandomness{0.8f};
    float wanderRandomness{initialWanderRandomness};
    std::unordered_map<ItemType, float> carriedItems;
    std::optional<FloatPosition> destination;
    bool hasReachedDestination = false;

public:
    Ant(AntRole role, int id, std::mt19937& rng);
    ~Ant();

    AntRole getRole() const;
    float getSize() const;
    sf::Color getColor() const;
    FloatPosition getPosition() const;
    void setPosition(FloatPosition newPosition);
    Vector2D getLastDirection() const;
    FloatPosition getPreviousPosition() const;
    float getWanderRandomness() const;
    float getCurrentLoad() const;
    float getMaxLoad() const;
    void setDestination(const FloatPosition& dest);

    void update(World& world);
    void move(const Vector2D& direction, World& world);
    bool pickUpItem(ItemType itemType, float amount);
    void dropItem(std::optional<ItemType> itemType = std::nullopt);
};
