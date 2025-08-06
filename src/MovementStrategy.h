#pragma once

#include <vector>
#include <algorithm>
#include "Position.h"
#include "Vector2D.h"

// Forward declarations
class Ant;
class World;
struct Vector2D;

// Base strategy class
class MovementStrategy {
protected:
    Vector2D getRandomDirection() const;
    Vector2D directionTowards(const FloatPosition& position, const FloatPosition& target) const;
    Vector2D addRandomnessToDirection(const Vector2D& direction, float randomness) const;
public:
    virtual Vector2D getMovementDirection(Ant& ant, World& world) = 0;
    virtual ~MovementStrategy() = default;
};

class QueenMovementStrategy : public MovementStrategy {
public:
    Vector2D getMovementDirection(Ant& ant, World& world) override;
};

class WorkerMovementStrategy : public MovementStrategy {
public:
    Vector2D getMovementDirection(Ant& ant, World& world) override;
};

class NurseMovementStrategy : public MovementStrategy {
public:
    Vector2D getMovementDirection(Ant& ant, World& world) override;
};

class ForagerMovementStrategy : public MovementStrategy {
public:
    Vector2D getMovementDirection(Ant& ant, World& world) override;
};

class SoldierMovementStrategy : public MovementStrategy {
public:
    Vector2D getMovementDirection(Ant& ant, World& world) override;
};

class DroneMovementStrategy : public MovementStrategy {
public:
    Vector2D getMovementDirection(Ant& ant, World& world) override;
};

class DefaultMovementStrategy : public MovementStrategy {
public:
    Vector2D getMovementDirection(Ant& ant, World& world) override;
};