#include <random>

#include "MovementStrategy.h"
#include "Vector2D.h"
#include "Position.h"


Vector2D MovementStrategy::getRandomDirection() const {
    std::uniform_real_distribution<float> dist(0, 2 * M_PI);
    float angle = dist(rng);
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

MovementDecision QueenMovementStrategy::decide(const SensoryInput& input) {
    if (input.distanceToNest > 0.5) {
        return { directionTowards(input.position, input.nestEntrancePosition), {} };
    }
    return { getRandomDirection() * 0.1f, {} };
}

MovementDecision WorkerMovementStrategy::decide(const SensoryInput& input) {
    return { addRandomnessToDirection(input.lastDirection, 0.2f), {} };
}

MovementDecision NurseMovementStrategy::decide(const SensoryInput& input) {
    return { addRandomnessToDirection(input.lastDirection, 0.5f), {} };
}

MovementDecision ForagerMovementStrategy::decide(const SensoryInput& input) {
    MovementDecision decision;
    float projectedLoad = input.currentLoad;

    if (input.onFood && projectedLoad < input.maxLoad && !input.onNestEntrance) {
        const float amountToPickUp = input.maxLoad - projectedLoad;
        decision.actions.push_back(movement_actions::PickUpItem{ItemType::FOOD, amountToPickUp});
        projectedLoad += amountToPickUp;
    }

    const bool carrying = projectedLoad >= input.maxLoad;
    if (carrying) {
        // Laying a trail back to the nest while returning.
        decision.actions.push_back(movement_actions::DepositPheromone{PheromoneType::FoodTrail, 8.0f});
        if (input.onNestEntrance) {
            decision.actions.push_back(movement_actions::DropItem{ItemType::FOOD});
        } else {
            decision.actions.push_back(movement_actions::SetDestination{input.nestEntrancePosition});
        }
    }

    Vector2D baseDirection = input.lastDirection;
    if (!carrying && input.foodTrailGradient.magnitude() > 0.001f) {
        // Follow the strongest scent; wander still mixes in via the randomness pass.
        baseDirection = input.foodTrailGradient;
    }
    decision.direction = addRandomnessToDirection(baseDirection, input.wanderRandomness);
    return decision;
}

MovementDecision SoldierMovementStrategy::decide(const SensoryInput& input) {
    return { addRandomnessToDirection(input.lastDirection, 0.4f), {} };
}

MovementDecision DroneMovementStrategy::decide(const SensoryInput& input) {
    return { addRandomnessToDirection(input.lastDirection, 0.1f), {} };
}

MovementDecision DefaultMovementStrategy::decide(const SensoryInput& input) {
    return { getRandomDirection(), {} };
}
