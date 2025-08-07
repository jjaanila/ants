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

Ant::Ant(AntRole role)
    : role(role),
      hasWings(role == AntRole::QUEEN || role == AntRole::DRONE),
      age(0),
      colony(""),
      energy(100.0),
      movementSpeed(0.0),
      eggLayingRate(0),
      attackPower(0.0),
      carryCapacity(0.0),
      nursingEfficiency(0.0),
      lastDirection(0.0f, 0.0f)
{
    id = UniqueIdGenerator::getNextId();
    foodPreferences = {"sugar", "protein", "seeds"};
    initializeRoleAttributes();
}

void Ant::initializeRoleAttributes() {
    weight = size / 2.0;
    switch (role) {
        case AntRole::QUEEN:
            color = sf::Color::Yellow;
            size = 10.0;  // Queens are larger
            lifespan = 365 * 15;  // Queens can live for years
            eggLayingRate = 1000; // Eggs per day
            hasWings = true;      // Queens start with wings but lose them
            movementSpeed = 5.0;
            movementStrategy = std::make_unique<QueenMovementStrategy>();
            break;
            
        case AntRole::WORKER:
            color = sf::Color::Black;
            size = 5.0;
            lifespan = 365;  // ~1 year
            carryCapacity = size * 2.0;
            movementSpeed = 10.0;
            movementStrategy = std::make_unique<WorkerMovementStrategy>();
            break;
            
        case AntRole::SOLDIER:
            color = sf::Color::Red;
            size = 7.5;  // Soldiers are larger than workers
            lifespan = 365;
            attackPower = size * 3.0;
            movementSpeed = 15.0;
            movementStrategy = std::make_unique<SoldierMovementStrategy>();
            break;
            
        case AntRole::DRONE:
            color = sf::Color::Cyan;
            size = 5.0;
            lifespan = 90;  // Shorter lifespan
            hasWings = true;
            movementSpeed = 5.0;
            movementStrategy = std::make_unique<DroneMovementStrategy>();
            break;
            
        case AntRole::FORAGER:
            color = sf::Color::Blue;
            size = 5.0;
            lifespan = 180;
            carryCapacity = size * 1.5;
            movementSpeed = 5.0;
            movementStrategy = std::make_unique<ForagerMovementStrategy>();
            break;
            
        case AntRole::NURSE:
            color = sf::Color::Green;
            size = 5.0;
            lifespan = 365;
            nursingEfficiency = 10.0;
            movementSpeed = 5.0;
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
    carryCapacity = 0.0;
    nursingEfficiency = 0.0;
    initializeRoleAttributes();
}

std::string Ant::getName() const {
    return "Ant " + std::to_string(id) + " (" + getRoleName() + ")";
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

// Getters and setters implementations
float Ant::getSize() const { return size; }
void Ant::setSize(float size) { this->size = size; }

sf::Color Ant::getColor() const { return color; }
void Ant::setColor(const sf::Color& color) { this->color = color; }

bool Ant::getHasWings() const { return hasWings; }
void Ant::setHasWings(bool hasWings) { this->hasWings = hasWings; }

std::string Ant::getColony() const { return colony; }
void Ant::setColony(const std::string& colony) { this->colony = colony; }

float Ant::getWanderRandomness() const { return wanderRandomness; }
Vector2D Ant::getLastDirection() const { return lastDirection; }

void Ant::update(World& world) {
    Vector2D moveDirection = movementStrategy->getMovementDirection(*this, world);
    move(moveDirection, world);
    
    lastDirection = moveDirection;
}

// Behavior methods implementations
void Ant::move(const Vector2D& direction, World& world) {
    FloatPosition newPosition = *position + direction * movementSpeed;
    if (world.isValidPosition(newPosition.getX(), newPosition.getY())) {
        previousPosition = std::make_unique<FloatPosition>(*position);
        position = std::make_unique<FloatPosition>(newPosition); 
    } else {
        // Hit obstacle, increase randomness for next move
        wanderRandomness = std::min(wanderRandomness + 0.1f, 1.0f);
    }
    float distance = direction.magnitude();
    float energyConsumption = distance * 0.1;
    if (role == AntRole::QUEEN) {
        energyConsumption *= 2.0;
    } else if (role == AntRole::SOLDIER) {
        energyConsumption *= 1.5;
    } else if (role == AntRole::FORAGER) {
        energyConsumption *= 0.8;
    }
    
    energy -= energyConsumption;
    if (energy < 0) energy = 0;
}

void Ant::forage() {
    if (role != AntRole::FORAGER && role != AntRole::WORKER) {
        std::cout << getRoleName() << " ants don't typically forage!" << std::endl;
        return;
    }
    
    energy -= 5.0;
    if (energy < 0) energy = 0;
    
    std::cout << getName() << " is foraging for food..." << std::endl;
    int successChance = (role == AntRole::FORAGER) ? 60 : 30;
    if (rand() % 100 < successChance) {
        std::cout << "Found food!" << std::endl;
    } else {
        std::cout << "No food found." << std::endl;
    }
}

void Ant::communicateWithPheromones(const std::string& message) {
    std::cout << getName() << " is leaving a pheromone trail: " << message << std::endl;
    energy -= 1.0;
    if (energy < 0) energy = 0;
}

void Ant::rest(int minutes) {
    std::cout << getName() << " is resting for " << minutes << " minutes." << std::endl;
    float recoveryRate = (role == AntRole::QUEEN) ? 1.0 : 0.5;
    
    energy += minutes * recoveryRate;
    if (energy > 100.0) energy = 100.0;
}

void Ant::defend() {
    if (role != AntRole::SOLDIER && role != AntRole::WORKER) {
        std::cout << getRoleName() << " ants don't typically defend the colony!" << std::endl;
        return;
    }
    
    if (role == AntRole::SOLDIER) {
        std::cout << "Soldier ant is attacking with power level " << attackPower << "!" << std::endl;
    } else {
        std::cout << "Worker ant is defending the colony!" << std::endl;
    }
    
    energy -= 10.0;
    if (energy < 0) energy = 0;
}

void Ant::buildNest() {
    if (role != AntRole::WORKER) {
        std::cout << getRoleName() << " ants don't typically build nests!" << std::endl;
        return;
    }
    
    std::cout << getName() << " is contributing to nest building." << std::endl;
    energy -= 8.0;
    if (energy < 0) energy = 0;
}

void Ant::layEggs(int count) {
    if (role != AntRole::QUEEN) {
        std::cout << "Only queen ants can lay eggs!" << std::endl;
        return;
    }
    
    if (count > eggLayingRate) {
        std::cout << "Queen can only lay up to " << eggLayingRate << " eggs per day." << std::endl;
        count = eggLayingRate;
    }
    
    std::cout << getName() << " is laying " << count << " eggs." << std::endl;
    energy -= count * 0.1;  // Energy consumption per egg
    if (energy < 0) energy = 0;
}

void Ant::nurseYoung() {
    if (role != AntRole::NURSE && role != AntRole::WORKER) {
        std::cout << getRoleName() << " ants don't typically nurse young!" << std::endl;
        return;
    }
    
    float efficiency = (role == AntRole::NURSE) ? nursingEfficiency : nursingEfficiency / 2;
    std::cout << getName() << " is nursing young with efficiency " 
              << efficiency << "." << std::endl;
    energy -= 5.0;
    if (energy < 0) energy = 0;
}

void Ant::mate() {
    if (role != AntRole::QUEEN && role != AntRole::DRONE) {
        std::cout << "Only queens and drones can mate!" << std::endl;
        return;
    }
    
    if (role == AntRole::QUEEN) {
        std::cout << "Queen ant is mating. Will store sperm to fertilize eggs." << std::endl;
    } else {
        std::cout << "Drone ant is mating. This is his life purpose!" << std::endl;
        energy -= 50.0;  // Mating is exhausting for drones
        if (energy < 0) energy = 0;
    }
}

void Ant::eatFood(const std::string& foodType, float amount) {
    std::cout << getName() << " is eating " << amount << "mg of " << foodType << std::endl;
    bool preferred = false;
    for (const auto& food : foodPreferences) {
        if (food == foodType) {
            preferred = true;
            break;
        }
    }
    if (preferred) {
        energy += amount * 2.0;
    } else {
        energy += amount;
    }
    if (role == AntRole::QUEEN) {
        energy += amount * 0.5;  // Extra energy for queens
    }
    
    if (energy > 100.0) energy = 100.0;
}

// Status methods implementations
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
    std::cout << "=== Ant Status ===" << std::endl;
    std::cout << "Role: " << getRoleName() << std::endl;
    std::cout << "Size: " << size << " mm" << std::endl;
    std::cout << "Color: " << color.r << color.b << color.g << color.a << std::endl;
    std::cout << "Has wings: " << (hasWings ? "Yes" : "No") << std::endl;
    std::cout << "Age: " << age << " days" << std::endl;
    std::cout << "Lifespan: " << lifespan << " days" << std::endl;
    std::cout << "Colony: " << (colony.empty() ? "None" : colony) << std::endl;
    const auto position = getPosition();
    std::cout << "Position: (" << position.getX() << ", " << position.getY() << ")" << std::endl;
    std::cout << "Energy level: " << energy << "%" << std::endl;
    std::cout << "Status: " << (isAlive() ? "Alive" : "Dead") << std::endl;
    switch (role) {
        case AntRole::QUEEN:
            std::cout << "Egg laying rate: " << eggLayingRate << " per day" << std::endl;
            break;
        case AntRole::SOLDIER:
            std::cout << "Attack power: " << attackPower << std::endl;
            break;
        case AntRole::WORKER:
        case AntRole::FORAGER:
            std::cout << "Carry capacity: " << carryCapacity << "mg" << std::endl;
            break;
        case AntRole::NURSE:
            std::cout << "Nursing efficiency: " << nursingEfficiency << std::endl;
            break;
        default:
            break;
    }
}