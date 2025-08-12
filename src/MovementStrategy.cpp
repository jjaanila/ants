#include <random>

#include "MovementStrategy.h"
#include "Vector2D.h"
#include "Position.h"
#include "World.h"


Vector2D MovementStrategy::getRandomDirection() const {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dist(0, 2 * M_PI);
    float angle = dist(gen);
    return Vector2D(std::cos(angle), std::sin(angle));
}

Vector2D MovementStrategy::directionTowards(const FloatPosition& position, const FloatPosition& target) const {
    const auto diff = (target - position).toVector2D();
    if (diff.magnitude() < 0.001f) return getRandomDirection();
    return diff.normalized();
}

Vector2D MovementStrategy::addRandomnessToDirection(const Vector2D& direction, float randomness) const {
    const auto randomComponent = getRandomDirection() * randomness;
    const auto result = direction * (1.0f - randomness) + randomComponent;
    return result.normalized();
}

Vector2D QueenMovementStrategy::getMovementDirection(Ant& ant, World& world) {
    const auto antPosition = ant.getPosition();
    if (world.distanceToNest(antPosition) > 0.5) {
        return directionTowards(antPosition, world.getNestEntrancePosition());
    }
    return getRandomDirection() * 0.1f;
}

// WorkerMovementStrategy implementation
Vector2D WorkerMovementStrategy::getMovementDirection(Ant& ant, World& world) {
    return addRandomnessToDirection(ant.getLastDirection(), 0.2f);
}

// NurseMovementStrategy implementation
Vector2D NurseMovementStrategy::getMovementDirection(Ant& ant, World& world) {
    return addRandomnessToDirection(ant.getLastDirection(), 0.5f);
}

// ForagerMovementStrategy implementation
Vector2D ForagerMovementStrategy::getMovementDirection(Ant& ant, World& world) {
    const auto antPosition = ant.getPosition();
    const auto tile = world.getTile(antPosition);
    const auto isNest = tile->getIsNestEntrance();
    if (tile->getHasFood()
        && ant.getCurrentLoad() < ant.getMaxLoad()
        && !isNest
    ) {
        float amountToPickUp = std::min(1.0f, ant.getMaxLoad() - ant.getCurrentLoad());
        if (ant.pickUpItem(ItemType::FOOD, amountToPickUp)) {
            ant.communicateWithPheromones("Found food");
        }
    }
    
    // Determine if forager should return to nest
    bool shouldReturnToNest = ant.getCurrentLoad() >= ant.getMaxLoad();
    
    // Update movement strategy based on carrying status
    if (shouldReturnToNest) {
        if (isNest) {
            ant.dropItem(ItemType::FOOD);
        } else {
            ant.setDestination(world.getNestEntrancePosition());
        }
    }
    return addRandomnessToDirection(ant.getLastDirection(), ant.getWanderRandomness());
}

// SoldierMovementStrategy implementation
Vector2D SoldierMovementStrategy::getMovementDirection(Ant& ant, World& world) {
    return addRandomnessToDirection(ant.getLastDirection(), 0.4f);
}

// DroneMovementStrategy implementation
Vector2D DroneMovementStrategy::getMovementDirection(Ant& ant, World& world) {
    return addRandomnessToDirection(ant.getLastDirection(), 0.1f);
}

// DefaultMovementStrategy implementation
Vector2D DefaultMovementStrategy::getMovementDirection(Ant& ant, World& world) {
    return getRandomDirection();
}