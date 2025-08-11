// Ant.cpp
#include "Ant.h"
#include "Id.h"
#include "MovementStrategy.h"
#include "Position.h"
#include "World.h"
#include <iostream>
#include <cmath>
#include <random>
#include <string>
#include <SFML/Graphics.hpp>

std::string itemTypeToString(ItemType type) {
    switch (type) {
        case ItemType::FOOD: return "food";
        case ItemType::LARVA: return "larva";
        case ItemType::EGG: return "egg";
        default: return "unknown item";
    }
}

Ant::Ant(AntRole role)
    : role(role),
      hasWings(role == AntRole::QUEEN || role == AntRole::DRONE),
      age(0),
      energy(100.0),
      movementSpeed(0.0),
      eggLayingRate(0),
      attackPower(0.0),
      maxLoad(0.0),
      nursingEfficiency(0.0),
      lastDirection(0.0f, 0.0f)
{
    id = UniqueIdGenerator::getNextId();
    foodPreferences = {"sugar", "protein", "seeds"};
    initializeRoleAttributes();
}

void Ant::initializeRoleAttributes() {
    weight = size / 2.0;
    const float baseSize = 0.5;
    const float baseMovementSpeed = 1;
    switch (role) {
        case AntRole::QUEEN:
            color = sf::Color::Yellow;
            size = baseSize * 2;  // Queens are larger
            lifespan = 365 * 15;  // Queens can live for years
            eggLayingRate = 1000; // Eggs per day
            hasWings = true;      // Queens start with wings but lose them
            movementSpeed = baseMovementSpeed;
            movementStrategy = std::make_unique<QueenMovementStrategy>();
            break;
            
        case AntRole::WORKER:
            color = sf::Color::Black;
            size = baseSize;
            lifespan = 365;  // ~1 year
            maxLoad = size * 2.0;
            movementSpeed = baseMovementSpeed;
            movementStrategy = std::make_unique<WorkerMovementStrategy>();
            break;
            
        case AntRole::SOLDIER:
            color = sf::Color::Red;
            size = baseSize * 1.3;  // Soldiers are larger than workers
            lifespan = 365;
            attackPower = size * 3.0;
            movementSpeed = baseMovementSpeed * 1.5;
            movementStrategy = std::make_unique<SoldierMovementStrategy>();
            break;
            
        case AntRole::DRONE:
            color = sf::Color::Cyan;
            size = baseSize;
            lifespan = 90;  // Shorter lifespan
            hasWings = true;
            movementSpeed = baseMovementSpeed;
            movementStrategy = std::make_unique<DroneMovementStrategy>();
            break;
            
        case AntRole::FORAGER:
            color = sf::Color::Blue;
            size = baseSize;
            lifespan = 180;
            maxLoad = size * 1.5;
            movementSpeed = baseMovementSpeed;
            movementStrategy = std::make_unique<ForagerMovementStrategy>();
            break;
            
        case AntRole::NURSE:
            color = sf::Color::Green;
            size = baseSize;
            lifespan = 365;
            nursingEfficiency = 10.0;
            movementSpeed = baseMovementSpeed;
            movementStrategy = std::make_unique<NurseMovementStrategy>();
            break;
    }
}

Ant::~Ant() {
}

// Role-related methods
AntRole Ant::getRole() const { return role; }

void Ant::setRole(AntRole newRole) { 
    role = newRole;
    updateAttributesBasedOnRole();
}

FloatPosition Ant::getPreviousPosition() const {
    return *previousPosition;
}

void Ant::updateAttributesBasedOnRole() {
    eggLayingRate = 0;
    attackPower = 0.0;
    maxLoad = 10;
    nursingEfficiency = 0.0;
    initializeRoleAttributes();
}

std::string Ant::getName() const {
    return "Ant " + std::to_string(id) + " (" + getRoleName() + ") ";
}

std::string Ant::getRoleName() const {
    switch (role) {
        case AntRole::QUEEN: return "Queen";
        case AntRole::WORKER: return "Worker";
        case AntRole::SOLDIER: return "Soldier";
        case AntRole::DRONE: return "Drone";
        case AntRole::FORAGER: return "Forager";
        case AntRole::NURSE: return "Nurse";
        default: return "Unknown";
    }
}

void Ant::log(const std::string& message) const {
    std::cout << getName() << message << std::endl;
}

// Getters and setters implementations
float Ant::getSize() const { return size; }
void Ant::setSize(float size) { this->size = size; }

sf::Color Ant::getColor() const { return color; }
void Ant::setColor(const sf::Color& color) { this->color = color; }

bool Ant::getHasWings() const { return hasWings; }
void Ant::setHasWings(bool hasWings) { this->hasWings = hasWings; }

float Ant::getWanderRandomness() const { return wanderRandomness; }
Vector2D Ant::getLastDirection() const { return lastDirection; }

float Ant::getCurrentLoad() const {
    float totalLoad = 0.0f;
    for (const auto& item : carriedItems) {
        totalLoad += item.second;
    }
    return totalLoad;
}

float Ant::getMaxLoad() const {
    return maxLoad;
}

const std::unordered_map<ItemType, float>& Ant::getCarriedItems() const {
    return carriedItems;
}

void Ant::update(World& world) {
    Vector2D moveDirection = movementStrategy->getMovementDirection(*this, world);
    move(moveDirection, world);
    
    lastDirection = moveDirection;
}

void Ant::move(const Vector2D& direction, World& world) {
    FloatPosition newPosition = *position + direction * movementSpeed;
    if (world.isValidPosition(newPosition)) {
        wanderRandomness = initialWanderRandomness;
        previousPosition = std::make_unique<FloatPosition>(*position);
        position = std::make_unique<FloatPosition>(newPosition); 
    } else {
        // Hit obstacle, increase randomness for next move
        wanderRandomness = std::min(wanderRandomness + 0.1f, 1.0f);
        previousPosition = std::make_unique<FloatPosition>(*position);
    }
}


bool Ant::pickUpItem(ItemType itemType, float amount) {
    // Only certain roles can carry items
    if (role != AntRole::WORKER && role != AntRole::FORAGER) {
        log("ants can't carry items!");
        return false;
    }
    
    // Calculate total weight currently carried
    float currentLoad = getCurrentLoad();
    
    // Check if adding this item would exceed carry capacity
    if (currentLoad + amount > maxLoad) {
        // Can't carry full amount, see if we can carry partial
        float remainingCapacity = maxLoad - currentLoad;
        
        if (remainingCapacity <= 0) {
            log("is already at maximum carrying capacity!");
            return false;
        }
        
        // Pick up partial amount
        float partialAmount = remainingCapacity;
        log("picked up " + std::to_string(partialAmount) + " of " 
                  + itemTypeToString(itemType) + " (at max capacity)");
        
        // Add to carried items or increase amount if already carrying this type
        carriedItems[itemType] += partialAmount;
        
        return true;  // Partial success
    }
    
    // Can carry full amount
    log("picked up " + std::to_string(amount) + " of " + itemTypeToString(itemType));
    
    // Add to carried items or increase amount if already carrying this type
    carriedItems[itemType] += amount;
    
    return true;
}

void Ant::dropItem(std::optional<ItemType> itemType) {
    if (!itemType.has_value()) {
        // Drop all items
        log("dropped all carried items.");
        carriedItems.clear();
    } else {
        // Drop specific item type
        auto it = carriedItems.find(itemType.value());
        if (it != carriedItems.end()) {
            log("dropped " + std::to_string(it->second) + " of " 
                      + itemTypeToString(itemType.value()));
            carriedItems.erase(it);
        } else {
            log("is not carrying any " 
                      + itemTypeToString(itemType.value()));
        }
    }
}

void Ant::communicateWithPheromones(const std::string& message) {
    log("is leaving a pheromone trail: " + message);
}

void Ant::rest(int minutes) {
    log("is resting for " + std::to_string(minutes) + " minutes.");
    float recoveryRate = (role == AntRole::QUEEN) ? 1.0 : 0.5;
}

void Ant::defend() {
    if (role != AntRole::SOLDIER && role != AntRole::WORKER) {
        log(getRoleName() + " ants don't typically defend the colony!");
        return;
    }
    
    if (role == AntRole::SOLDIER) {
        log("Soldier ant is attacking with power level " + std::to_string(attackPower) + "!");
    } else {
        log("Worker ant is defending the colony!");
    }
}

void Ant::buildNest() {
    if (role != AntRole::WORKER) {
        log(getRoleName() + " ants don't typically build nests!");
        return;
    }
    
    log("is contributing to nest building.");
}

void Ant::layEggs(int count) {
    if (role != AntRole::QUEEN) {
        log("Only queen ants can lay eggs!");
        return;
    }
    
    if (count > eggLayingRate) {
        log("Queen can only lay up to " + std::to_string(eggLayingRate) + " eggs per day.");
        count = eggLayingRate;
    }
    
    log("is laying " + std::to_string(count) + " eggs.");
}

void Ant::nurseYoung() {
    if (role != AntRole::NURSE && role != AntRole::WORKER) {
        log(getRoleName() + " ants don't typically nurse young!");
        return;
    }
    
    float efficiency = (role == AntRole::NURSE) ? nursingEfficiency : nursingEfficiency / 2;
    log("is nursing.");
}

void Ant::mate() {
    if (role != AntRole::QUEEN && role != AntRole::DRONE) {
        log("Only queens and drones can mate!");
        return;
    }
    
    if (role == AntRole::QUEEN) {
        log("Queen ant is mating. Will store sperm to fertilize eggs.");
    } else {
        log("Drone ant is mating. This is his life purpose!");
    }
}

void Ant::eatFood(const std::string& foodType, float amount) {
    log("is eating " + std::to_string(amount) + "mg of " + foodType);
}

bool Ant::isAlive() const {
    return energy > 0;
}

float Ant::getEnergyLevel() const {
    return energy;
}

FloatPosition Ant::getPosition() const {
    return *position;
}

void Ant::setPosition(FloatPosition newPosition) {
    position = std::make_unique<FloatPosition>(newPosition);
}

void Ant::displayStatus() const {
    log("=== Ant Status ===");
    log("Role: " + getRoleName());
    log("Size: " + std::to_string(size) + " mm");
    log("Color: " + std::to_string(color.r) + std::to_string(color.b) + std::to_string(color.g) + std::to_string(color.a));
    log("Has wings: " + std::string(hasWings ? "Yes" : "No"));
    log("Age: " + std::to_string(age) + " days");
    log("Lifespan: " + std::to_string(lifespan) + " days");
    const auto position = getPosition();
    log("Position: (" + std::to_string(position.getX()) + ", " + std::to_string(position.getY()) + ")");
    log("Energy level: " + std::to_string(energy) + "%");
    log("Status: " + std::string(isAlive() ? "Alive" : "Dead"));
    switch (role) {
        case AntRole::QUEEN:
            log("Egg laying rate: " + std::to_string(eggLayingRate) + " per day");
            break;
        case AntRole::SOLDIER:
            log("Attack power: " + std::to_string(attackPower));
            break;
        case AntRole::WORKER:
        case AntRole::FORAGER:
            log("Carry capacity: " + std::to_string(maxLoad) + "mg");
            break;
        case AntRole::NURSE:
            log("Nursing efficiency: " + std::to_string(nursingEfficiency));
            break;
        default:
            break;
    }
}