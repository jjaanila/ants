#pragma once

#include <vector>
#include <variant>
#include <optional>
#include <string>
#include <algorithm>
#include "Position.h"
#include "Vector2D.h"
#include "Ant.h"

// Forward declarations
class Ant;
class World;
struct Vector2D;

namespace movement_actions {
    struct PickUpItem    { ItemType itemType; float amount; };
    struct DropItem      { std::optional<ItemType> itemType; };
    struct StartPheromone { std::string message; };
    struct StopPheromone  { std::string message; };
    struct SetDestination { FloatPosition destination; };
}

using MovementAction = std::variant<
    movement_actions::PickUpItem,
    movement_actions::DropItem,
    movement_actions::StartPheromone,
    movement_actions::StopPheromone,
    movement_actions::SetDestination
>;

struct MovementDecision {
    Vector2D direction;
    std::vector<MovementAction> actions;
};

// Base strategy class
class MovementStrategy {
protected:
    Vector2D getRandomDirection() const;
    Vector2D directionTowards(const FloatPosition& position, const FloatPosition& target) const;
    Vector2D addRandomnessToDirection(const Vector2D& direction, float randomness) const;
public:
    virtual MovementDecision decide(const Ant& ant, World& world) = 0;
    virtual ~MovementStrategy() = default;
};

class QueenMovementStrategy : public MovementStrategy {
public:
    MovementDecision decide(const Ant& ant, World& world) override;
};

class WorkerMovementStrategy : public MovementStrategy {
public:
    MovementDecision decide(const Ant& ant, World& world) override;
};

class NurseMovementStrategy : public MovementStrategy {
public:
    MovementDecision decide(const Ant& ant, World& world) override;
};

class ForagerMovementStrategy : public MovementStrategy {
public:
    MovementDecision decide(const Ant& ant, World& world) override;
};

class SoldierMovementStrategy : public MovementStrategy {
public:
    MovementDecision decide(const Ant& ant, World& world) override;
};

class DroneMovementStrategy : public MovementStrategy {
public:
    MovementDecision decide(const Ant& ant, World& world) override;
};

class DefaultMovementStrategy : public MovementStrategy {
public:
    MovementDecision decide(const Ant& ant, World& world) override;
};
