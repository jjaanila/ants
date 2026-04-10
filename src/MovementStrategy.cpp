#include <random>

#include "MovementStrategy.h"
#include "Vector2D.h"
#include "Position.h"
#include "World.h"
#include "Ant.h"
#include "Tile.h"


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

MovementDecision QueenMovementStrategy::decide(const Ant& ant, World& world) {
    const auto antPosition = ant.getPosition();
    if (world.distanceToNest(antPosition) > 0.5) {
        return { directionTowards(antPosition, world.getNestEntrancePosition()), {} };
    }
    return { getRandomDirection() * 0.1f, {} };
}

MovementDecision WorkerMovementStrategy::decide(const Ant& ant, World& world) {
    return { addRandomnessToDirection(ant.getLastDirection(), 0.2f), {} };
}

MovementDecision NurseMovementStrategy::decide(const Ant& ant, World& world) {
    return { addRandomnessToDirection(ant.getLastDirection(), 0.5f), {} };
}

MovementDecision ForagerMovementStrategy::decide(const Ant& ant, World& world) {
    MovementDecision decision;
    const auto antPosition = ant.getPosition();
    const auto tile = world.getTile(antPosition);
    const bool isNest = tile->getIsNestEntrance();

    // Track current load symbolically so the return-to-nest decision reflects
    // the pickup we're about to emit this tick.
    float projectedLoad = ant.getCurrentLoad();
    const float maxLoad = ant.getMaxLoad();

    if (tile->getHasFood() && projectedLoad < maxLoad && !isNest) {
        const float amountToPickUp = maxLoad - projectedLoad;
        decision.actions.push_back(movement_actions::PickUpItem{ItemType::FOOD, amountToPickUp});
        projectedLoad += amountToPickUp;
        // Preserves prior behavior: pickUpItem doesn't mutate tile food state,
        // so this branch is evaluated against the pre-pickup tile.
        if (tile->getHasFood()) {
            decision.actions.push_back(movement_actions::StartPheromone{"Found food"});
        } else {
            decision.actions.push_back(movement_actions::StopPheromone{"Found food"});
        }
    }

    const bool shouldReturnToNest = projectedLoad >= maxLoad;
    if (shouldReturnToNest) {
        if (isNest) {
            decision.actions.push_back(movement_actions::DropItem{ItemType::FOOD});
        } else {
            decision.actions.push_back(movement_actions::SetDestination{world.getNestEntrancePosition()});
        }
    }

    decision.direction = addRandomnessToDirection(ant.getLastDirection(), ant.getWanderRandomness());
    return decision;
}

MovementDecision SoldierMovementStrategy::decide(const Ant& ant, World& world) {
    return { addRandomnessToDirection(ant.getLastDirection(), 0.4f), {} };
}

MovementDecision DroneMovementStrategy::decide(const Ant& ant, World& world) {
    return { addRandomnessToDirection(ant.getLastDirection(), 0.1f), {} };
}

MovementDecision DefaultMovementStrategy::decide(const Ant& ant, World& world) {
    return { getRandomDirection(), {} };
}
