#pragma once

#include <vector>
#include <variant>
#include <optional>
#include <random>
#include <string>
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

// Snapshot of everything a strategy is allowed to observe about the ant and
// its surroundings. Strategies read only from this DTO — they do not touch
// Ant or World directly, so they can be unit-tested without either.
struct SensoryInput {
    // Ant self-state
    FloatPosition position;
    Vector2D lastDirection;
    float currentLoad;
    float maxLoad;
    float wanderRandomness;

    // Immediate tile under the ant
    bool onFood;
    bool onNestEntrance;

    // Nest-relative
    float distanceToNest;
    FloatPosition nestEntrancePosition;
};

// Base strategy class
class MovementStrategy {
protected:
    std::mt19937& rng;
    Vector2D getRandomDirection() const;
    Vector2D directionTowards(const FloatPosition& position, const FloatPosition& target) const;
    Vector2D addRandomnessToDirection(const Vector2D& direction, float randomness) const;
public:
    explicit MovementStrategy(std::mt19937& rng) : rng(rng) {}
    virtual MovementDecision decide(const SensoryInput& input) = 0;
    virtual ~MovementStrategy() = default;
};

class QueenMovementStrategy : public MovementStrategy {
public:
    using MovementStrategy::MovementStrategy;
    MovementDecision decide(const SensoryInput& input) override;
};

class WorkerMovementStrategy : public MovementStrategy {
public:
    using MovementStrategy::MovementStrategy;
    MovementDecision decide(const SensoryInput& input) override;
};

class NurseMovementStrategy : public MovementStrategy {
public:
    using MovementStrategy::MovementStrategy;
    MovementDecision decide(const SensoryInput& input) override;
};

class ForagerMovementStrategy : public MovementStrategy {
public:
    using MovementStrategy::MovementStrategy;
    MovementDecision decide(const SensoryInput& input) override;
};

class SoldierMovementStrategy : public MovementStrategy {
public:
    using MovementStrategy::MovementStrategy;
    MovementDecision decide(const SensoryInput& input) override;
};

class DroneMovementStrategy : public MovementStrategy {
public:
    using MovementStrategy::MovementStrategy;
    MovementDecision decide(const SensoryInput& input) override;
};

class DefaultMovementStrategy : public MovementStrategy {
public:
    using MovementStrategy::MovementStrategy;
    MovementDecision decide(const SensoryInput& input) override;
};
