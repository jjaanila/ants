// Ant.h
#ifndef ANT_H
#define ANT_H

#include <string>
#include <vector>
#include <memory>

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
    double sizeInMm;
    std::string color;
    double weight; // in milligrams
    bool hasWings;
    int age; // in days
    std::string colony;
    std::vector<std::string> foodPreferences;
    int lifespan; // in days, varies by role
    
    // Current status
    double energy; // 0.0 to 100.0
    std::pair<double, double> position; // x, y coordinates
    
    // Role-specific attributes
    int eggLayingRate;  // For queens, eggs per day
    double attackPower; // For soldiers
    double carryCapacity; // For workers and foragers
    double nursingEfficiency; // For nurses

public:
    // Constructor
    Ant(AntRole role = AntRole::WORKER,
        double size = 5.0, 
        const std::string& color = "black");
    
    // Destructor
    ~Ant();
    
    // Basic getters and setters
    AntRole getRole() const;
    void setRole(AntRole role);
    std::string getName() const;
    std::string getRoleName() const; // Returns role as string
    
    double getSize() const;
    void setSize(double sizeInMm);
    
    std::string getColor() const;
    void setColor(const std::string& color);
    
    bool getHasWings() const;
    void setHasWings(bool hasWings);
    
    std::string getColony() const;
    void setColony(const std::string& colony);
    
    // General behavior methods
    void move(double x, double y);
    void rest(int minutes);
    void eatFood(const std::string& foodType, double amount);
    
    // Role-specific behavior methods
    void forage();  // Primarily for foragers
    void returnToNest();
    void communicateWithPheromones(const std::string& message);
    void defend();  // Primarily for soldiers
    void buildNest();  // Primarily for workers
    void layEggs(int count);  // Only for queens
    void nurseYoung();  // Only for nurses
    void mate();  // Only for queens and drones
    
    // Status methods
    bool isAlive() const;
    double getEnergyLevel() const;
    std::pair<double, double> getPosition() const;
    void displayStatus() const;
    
private:
    // Helper methods
    void initializeRoleAttributes();
    void updateAttributesBasedOnRole();
};

#endif // ANT_H