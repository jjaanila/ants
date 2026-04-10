#include "Ant.h"
#include "MovementStrategy.h"
#include "Position.h"
#include "Tile.h"
#include "World.h"

#include <algorithm>
#include <type_traits>
#include <variant>

namespace {

struct AntRoleConfig {
    sf::Color color;
    float size;
    float movementSpeed;
    float maxLoad;
};

constexpr float kBaseSize = 0.5f;
constexpr float kBaseMovementSpeed = 1.0f;

AntRoleConfig configForRole(AntRole role) {
    switch (role) {
        case AntRole::QUEEN:
            return { sf::Color::Yellow, kBaseSize * 2.0f, kBaseMovementSpeed, 0.0f };
        case AntRole::WORKER:
            return { sf::Color::Black, kBaseSize, kBaseMovementSpeed, kBaseSize * 2.0f };
        case AntRole::SOLDIER:
            return { sf::Color::Red, kBaseSize * 1.3f, kBaseMovementSpeed * 1.5f, 0.0f };
        case AntRole::DRONE:
            return { sf::Color::Cyan, kBaseSize, kBaseMovementSpeed, 0.0f };
        case AntRole::FORAGER:
            return { sf::Color::Blue, kBaseSize, kBaseMovementSpeed, kBaseSize * 1.5f };
        case AntRole::NURSE:
            return { sf::Color::Green, kBaseSize, kBaseMovementSpeed, 0.0f };
    }
    return { sf::Color::White, kBaseSize, kBaseMovementSpeed, 0.0f };
}

std::unique_ptr<MovementStrategy> makeStrategyFor(AntRole role, std::mt19937& rng) {
    switch (role) {
        case AntRole::QUEEN:   return std::make_unique<QueenMovementStrategy>(rng);
        case AntRole::WORKER:  return std::make_unique<WorkerMovementStrategy>(rng);
        case AntRole::SOLDIER: return std::make_unique<SoldierMovementStrategy>(rng);
        case AntRole::DRONE:   return std::make_unique<DroneMovementStrategy>(rng);
        case AntRole::FORAGER: return std::make_unique<ForagerMovementStrategy>(rng);
        case AntRole::NURSE:   return std::make_unique<NurseMovementStrategy>(rng);
    }
    return std::make_unique<DefaultMovementStrategy>(rng);
}

} // namespace

Ant::Ant(AntRole role, int id, std::mt19937& rng)
    : rng(rng),
      id(id),
      role(role),
      lastDirection(0.0f, 0.0f)
{
    const auto config = configForRole(role);
    size = config.size;
    color = config.color;
    movementSpeed = config.movementSpeed;
    maxLoad = config.maxLoad;
    movementStrategy = makeStrategyFor(role, rng);
}

Ant::~Ant() = default;

AntRole Ant::getRole() const { return role; }
float Ant::getSize() const { return size; }
sf::Color Ant::getColor() const { return color; }
Vector2D Ant::getLastDirection() const { return lastDirection; }
float Ant::getWanderRandomness() const { return wanderRandomness; }
float Ant::getMaxLoad() const { return maxLoad; }

FloatPosition Ant::getPosition() const { return *position; }
void Ant::setPosition(FloatPosition newPosition) {
    position = std::make_unique<FloatPosition>(newPosition);
}
FloatPosition Ant::getPreviousPosition() const { return *previousPosition; }

void Ant::setDestination(const FloatPosition& dest) {
    destination = dest;
    hasReachedDestination = false;
}

float Ant::getCurrentLoad() const {
    float total = 0.0f;
    for (const auto& item : carriedItems) {
        total += item.second;
    }
    return total;
}

void Ant::update(World& world) {
    const auto currentPosition = getPosition();
    const auto tile = world.getTile(currentPosition);

    const IntegerPosition tilePos = tile->getPosition();
    auto trailAt = [&world](int x, int y) {
        const Tile* t = world.getTile(IntegerPosition(x, y));
        return t ? t->getPheromone(PheromoneType::FoodTrail) : 0.0f;
    };
    const float trailE = trailAt(tilePos.getX() + 1, tilePos.getY());
    const float trailW = trailAt(tilePos.getX() - 1, tilePos.getY());
    const float trailS = trailAt(tilePos.getX(), tilePos.getY() + 1);
    const float trailN = trailAt(tilePos.getX(), tilePos.getY() - 1);
    Vector2D gradient(trailE - trailW, trailS - trailN);
    if (gradient.magnitude() > 0.001f) {
        gradient = gradient.normalized();
    } else {
        gradient = Vector2D(0.0f, 0.0f);
    }

    const SensoryInput input{
        .position = currentPosition,
        .lastDirection = lastDirection,
        .currentLoad = getCurrentLoad(),
        .maxLoad = maxLoad,
        .wanderRandomness = wanderRandomness,
        .onFood = tile->getHasFood(),
        .onNestEntrance = tile->getIsNestEntrance(),
        .foodTrailHere = tile->getPheromone(PheromoneType::FoodTrail),
        .foodTrailGradient = gradient,
        .distanceToNest = world.distanceToNest(currentPosition),
        .nestEntrancePosition = world.getNestEntrancePosition(),
    };

    MovementDecision decision = movementStrategy->decide(input);

    for (const auto& action : decision.actions) {
        std::visit([this, &world, &tilePos](const auto& a) {
            using T = std::decay_t<decltype(a)>;
            if constexpr (std::is_same_v<T, movement_actions::PickUpItem>) {
                this->pickUpItem(a.itemType, a.amount);
            } else if constexpr (std::is_same_v<T, movement_actions::DropItem>) {
                this->dropItem(a.itemType);
            } else if constexpr (std::is_same_v<T, movement_actions::DepositPheromone>) {
                world.depositPheromone(tilePos, a.type, a.amount);
            } else if constexpr (std::is_same_v<T, movement_actions::SetDestination>) {
                this->setDestination(a.destination);
            }
        }, action);
    }

    move(decision.direction, world);
    lastDirection = decision.direction;
}

void Ant::move(const Vector2D& direction, World& world) {
    if (destination.has_value() && !hasReachedDestination) {
        const FloatPosition& dest = destination.value();
        const float distanceToDestination = position->distanceTo(dest);

        if (distanceToDestination <= movementSpeed) {
            previousPosition = std::make_unique<FloatPosition>(*position);
            position = std::make_unique<FloatPosition>(dest);
            hasReachedDestination = true;
            return;
        }

        const Vector2D directionToDestination = Vector2D(
            dest.getX() - position->getX(),
            dest.getY() - position->getY()
        ).normalized();

        const FloatPosition newPosition = *position + directionToDestination * movementSpeed;
        if (world.isValidPosition(newPosition)) {
            wanderRandomness = initialWanderRandomness;
            previousPosition = std::make_unique<FloatPosition>(*position);
            position = std::make_unique<FloatPosition>(newPosition);
        } else {
            wanderRandomness = std::min(wanderRandomness + 0.1f, 1.0f);
            previousPosition = std::make_unique<FloatPosition>(*position);
        }
    } else {
        const FloatPosition newPosition = *position + direction * movementSpeed;
        if (world.isValidPosition(newPosition)) {
            wanderRandomness = initialWanderRandomness;
            previousPosition = std::make_unique<FloatPosition>(*position);
            position = std::make_unique<FloatPosition>(newPosition);
        } else {
            wanderRandomness = std::min(wanderRandomness + 0.1f, 1.0f);
            previousPosition = std::make_unique<FloatPosition>(*position);
        }
    }
}

bool Ant::pickUpItem(ItemType itemType, float amount) {
    if (role != AntRole::WORKER && role != AntRole::FORAGER) return false;

    const float currentLoad = getCurrentLoad();
    if (currentLoad >= maxLoad) return false;

    const float remainingCapacity = maxLoad - currentLoad;
    const float taken = std::min(amount, remainingCapacity);
    carriedItems[itemType] += taken;
    return true;
}

void Ant::dropItem(std::optional<ItemType> itemType) {
    if (!itemType.has_value()) {
        carriedItems.clear();
    } else {
        carriedItems.erase(itemType.value());
    }
}
