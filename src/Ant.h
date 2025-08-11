#pragma once

#include <string>
#include <vector>
#include <memory>

#include "SFML/Graphics.hpp"
#include "Position.h"

// Forward declarations
class MovementStrategy;
class World;

enum class ItemType {
    FOOD,
    LARVA,
    EGG,
};

std::string itemTypeToString(ItemType type);

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
    float size;
    sf::Color color;
    float weight; // in milligrams
    bool hasWings;
    int age; // in days
    std::vector<std::string> foodPreferences;
    int lifespan; // in days, varies by role
    float energy; // 0.0 to 100.0
    std::unique_ptr<FloatPosition> position;
    std::unique_ptr<FloatPosition> previousPosition;
    float movementSpeed;
    std::unique_ptr<MovementStrategy> movementStrategy;
    int eggLayingRate;  // For queens, eggs per day
    float attackPower; // For soldiers
    float maxLoad; // For workers and foragers
    float nursingEfficiency; // For nurses
    Vector2D lastDirection;
    float initialWanderRandomness{0.8f};
    float wanderRandomness{initialWanderRandomness};
    std::unordered_map<ItemType, float> carriedItems;  // Maps item type to amount/weight
    const std::unordered_map<ItemType, float>& getCarriedItems() const;

public:
    Ant(AntRole role);
    ~Ant();
    AntRole getRole() const;
    void setRole(AntRole role);
    std::string getName() const;
    std::string getRoleName() const; // Returns role as string
    float getSize() const;
    void setSize(float size);
    sf::Color getColor() const;
    void setColor(const sf::Color& color);
    bool getHasWings() const;
    void setHasWings(bool hasWings);
    float getEnergyLevel() const;
    FloatPosition getPosition() const;
    void setPosition(FloatPosition newPosition);
    Vector2D getLastDirection() const;
    FloatPosition getPreviousPosition() const;
    float getWanderRandomness() const;
    float getCurrentLoad() const;
    float getMaxLoad() const;
    void log(const std::string& message) const;

    void update(World& world);
    void move(const Vector2D& direction, World& world);
    void rest(int minutes);
    void eatFood(const std::string& foodType, float amount);
    bool pickUpItem(ItemType itemType, float amount);
    void dropItem(std::optional<ItemType> itemType = std::nullopt);
    void communicateWithPheromones(const std::string& message);
    void defend();  // Primarily for soldiers
    void buildNest();  // Primarily for workers
    void layEggs(int count);  // Only for queens
    void nurseYoung();  // Only for nurses
    void mate();  // Only for queens and drones
    bool isAlive() const;
    void displayStatus() const;
    
private:
    void initializeRoleAttributes();
    void updateAttributesBasedOnRole();
};
