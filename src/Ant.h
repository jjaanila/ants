// Ant.h
#pragma once

#include <string>
#include <vector>
#include <memory>

#include "SFML/Graphics.hpp"
#include "Position.h"

// Forward declarations
class MovementStrategy;
class World;

// Define ant roles as an enum
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
    int id;
    AntRole role;
    float sizeInMm;
    std::string color;
    float weight; // in milligrams
    bool hasWings;
    int age; // in days
    std::string colony;
    std::vector<std::string> foodPreferences;
    int lifespan; // in days, varies by role
    float energy; // 0.0 to 100.0
    std::unique_ptr<FloatPosition> position;
    float movementSpeed;
    std::unique_ptr<MovementStrategy> movementStrategy;
    int eggLayingRate;  // For queens, eggs per day
    float attackPower; // For soldiers
    float carryCapacity; // For workers and foragers
    float nursingEfficiency; // For nurses
    Vector2D lastDirection;
    float wanderRandomness{0.8f};

public:
    Ant(AntRole role = AntRole::WORKER,
        float size = 5.0, 
        const std::string& color = "black");
    ~Ant();
    AntRole getRole() const;
    void setRole(AntRole role);
    std::string getName() const;
    std::string getRoleName() const; // Returns role as string
    float getSize() const;
    void setSize(float sizeInMm);
    std::string getColor() const;
    void setColor(const std::string& color);
    bool getHasWings() const;
    void setHasWings(bool hasWings);
    std::string getColony() const;
    void setColony(const std::string& colony);
    float getEnergyLevel() const;
    FloatPosition getPosition() const;
    void setPosition(FloatPosition newPosition);
    Vector2D getLastDirection() const;
    float getWanderRandomness() const;

    void update(World& world);
    void move(const Vector2D& direction, World& world);
    void rest(int minutes);
    void eatFood(const std::string& foodType, float amount);
    void forage();  // Primarily for foragers
    void returnToNest();
    void communicateWithPheromones(const std::string& message);
    void defend();  // Primarily for soldiers
    void buildNest();  // Primarily for workers
    void layEggs(int count);  // Only for queens
    void nurseYoung();  // Only for nurses
    void mate();  // Only for queens and drones
    void wander();
    bool isAlive() const;
    void displayStatus() const;
    sf::RectangleShape draw() const;
    
private:
    void initializeRoleAttributes();
    void updateAttributesBasedOnRole();
};
