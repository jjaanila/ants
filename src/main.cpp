// main.cpp - Ant Colony Simulator with main simulation loop
#include "Ant.h"
#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <chrono>
#include <random>
#include <cstdlib>
#include <ctime>
#include <iomanip>

// Function to clear the screen (works on most terminals)
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Function to print a separator line
void printSeparator(char symbol = '-', int length = 80) {
    std::cout << std::string(length, symbol) << std::endl;
}

// Random number generator
std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));

// Return random int in range [min, max]
int getRandomInt(int min, int max) {
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(rng);
}

// Return random double in range [min, max]
double getRandomDouble(double min, double max) {
    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(rng);
}

// Simulation parameters
const int COLONY_SIZE = 20;         // Number of ants in the simulation
const int WORLD_SIZE = 100;         // Size of the world (grid)
const int MAX_ITERATIONS = 100;     // Maximum number of simulation iterations
const int DELAY_MS = 500;           // Delay between iterations (milliseconds)

int main() {
    // Seed random number generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    // Create colony of ants
    std::vector<std::unique_ptr<Ant>> colony;
    
    // Add a queen (always have at least one queen)
    colony.push_back(std::make_unique<Ant>(AntRole::QUEEN, 8.0, "dark red"));
    colony.back()->setColony("Fire Nation");
    
    // Add random ants with different roles to the colony
    for (int i = 1; i < COLONY_SIZE; ++i) {
        // Determine role with appropriate distribution
        // (more workers than other roles, only one queen, few drones)
        int roleRandom = getRandomInt(1, 100);
        AntRole role;
        
        if (roleRandom <= 45) {
            role = AntRole::WORKER;
        } else if (roleRandom <= 70) {
            role = AntRole::FORAGER;
        } else if (roleRandom <= 85) {
            role = AntRole::SOLDIER;
        } else if (roleRandom <= 95) {
            role = AntRole::NURSE;
        } else {
            role = AntRole::DRONE;
        }
        
        // Create ant with random but appropriate attributes
        double size = getRandomDouble(4.0, 7.0);  // Size in mm
        
        // Color based on role
        std::string color;
        switch (role) {
            case AntRole::WORKER: color = "red"; break;
            case AntRole::FORAGER: color = "light red"; break;
            case AntRole::SOLDIER: color = "dark red"; break;
            case AntRole::NURSE: color = "orange"; break;
            case AntRole::DRONE: color = "black"; break;
            default: color = "brown"; break;
        }
        
        colony.push_back(std::make_unique<Ant>(role, size, color));
        colony.back()->setColony("Fire Nation");
        
        // Place ants at random positions in the world
        double x = getRandomDouble(0, WORLD_SIZE);
        double y = getRandomDouble(0, WORLD_SIZE);
        colony.back()->move(x, y);
    }
    
    // Main simulation loop
    int iteration = 0;
    bool running = true;
    
    std::cout << "Starting Ant Colony Simulation with " << COLONY_SIZE << " ants\n";
    std::cout << "Press Ctrl+C to exit\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    while (running && iteration < MAX_ITERATIONS) {
        clearScreen();
        
        std::cout << "=== Ant Colony Simulation - Iteration " << iteration + 1 
                  << "/" << MAX_ITERATIONS << " ===\n\n";
        
        // Count ants by role
        int queenCount = 0, workerCount = 0, soldierCount = 0, 
            foragerCount = 0, nurseCount = 0, droneCount = 0;
            
        for (const auto& ant : colony) {
            switch (ant->getRole()) {
                case AntRole::QUEEN: queenCount++; break;
                case AntRole::WORKER: workerCount++; break;
                case AntRole::SOLDIER: soldierCount++; break;
                case AntRole::FORAGER: foragerCount++; break;
                case AntRole::NURSE: nurseCount++; break;
                case AntRole::DRONE: droneCount++; break;
            }
        }
        
        // Display colony statistics
        std::cout << "Colony Statistics:\n";
        std::cout << "Queens:   " << std::setw(3) << queenCount << " | ";
        std::cout << "Workers:  " << std::setw(3) << workerCount << " | ";
        std::cout << "Soldiers: " << std::setw(3) << soldierCount << "\n";
        std::cout << "Foragers: " << std::setw(3) << foragerCount << " | ";
        std::cout << "Nurses:   " << std::setw(3) << nurseCount << " | ";
        std::cout << "Drones:   " << std::setw(3) << droneCount << "\n\n";
        
        printSeparator();
        std::cout << "Activity Log:\n";
        
        for (auto& ant : colony) {
            if (!ant->isAlive()) {
                std::cout << ant->getRoleName() << " ant has died!\n";
                continue;
            }
            
            int decisionFactor = getRandomInt(1, 100);
            
            switch (ant->getRole()) {
                case AntRole::QUEEN:
                    if (decisionFactor <= 70) {
                        ant->layEggs(getRandomInt(100, 300));
                    } else if (decisionFactor <= 85) {
                        ant->rest(getRandomInt(10, 30));
                    } else if (decisionFactor <= 90) {
                        ant->mate();
                    } else {
                        double x = getRandomDouble(0, 10);
                        double y = getRandomDouble(0, 10);
                        ant->move(x, y);
                    }
                    break;
                    
                case AntRole::WORKER:
                    if (decisionFactor <= 50) {
                        ant->buildNest();
                    } else if (decisionFactor <= 80) {
                        ant->forage();
                        if (getRandomInt(1, 100) > 70) {
                            ant->returnToNest();
                        }
                    } else {
                        ant->rest(getRandomInt(5, 15));
                    }
                    break;
                    
                case AntRole::SOLDIER:
                    // Soldiers defend and patrol
                    if (decisionFactor <= 60) {
                        ant->defend();
                    } else {
                        // Patrol (move around)
                        double x = getRandomDouble(0, WORLD_SIZE);
                        double y = getRandomDouble(0, WORLD_SIZE);
                        ant->move(x, y);
                    }
                    break;
                    
                case AntRole::FORAGER:
                    if (decisionFactor <= 70) {
                        ant->forage();
                        if (getRandomInt(1, 100) <= 60) {
                            ant->communicateWithPheromones("Food found!");
                            ant->returnToNest();
                        }
                    } else {
                        // Move to a new location to forage
                        double x = getRandomDouble(0, WORLD_SIZE);
                        double y = getRandomDouble(0, WORLD_SIZE);
                        ant->move(x, y);
                    }
                    break;
                    
                case AntRole::NURSE:
                    if (decisionFactor <= 70) {
                        ant->nurseYoung();
                    } else if (decisionFactor <= 90) {
                        ant->rest(getRandomInt(5, 15));
                    } else {
                        ant->eatFood("protein", getRandomDouble(0.5, 2.0));
                    }
                    break;
                    
                case AntRole::DRONE:
                    if (decisionFactor <= 20) {
                        ant->mate();
                    } else if (decisionFactor <= 40) {
                        ant->eatFood("sugar", getRandomDouble(0.5, 1.5));
                    } else {
                        double x = getRandomDouble(0, WORLD_SIZE);
                        double y = getRandomDouble(0, WORLD_SIZE);
                        ant->move(x, y);
                    }
                    break;
            }
            
            // Random chance for ants to eat
            if (getRandomInt(1, 100) <= 20) {
                std::vector<std::string> foods = {"sugar", "protein", "seeds"};
                ant->eatFood(foods[getRandomInt(0, 2)], getRandomDouble(0.5, 2.0));
            }
        }
        
        // Add some environmental events that affect the colony
        if (getRandomInt(1, 100) <= 10) {
            // Random environmental event
            int eventType = getRandomInt(1, 3);
            
            printSeparator('*');
            switch (eventType) {
                case 2:
                    std::cout << "COLONY EVENT: Predator approaching!\n";
                    // Soldiers defend
                    for (auto& ant : colony) {
                        if (ant->getRole() == AntRole::SOLDIER) {
                            ant->defend();
                        }
                    }
                    break;
                case 3:
                    std::cout << "COLONY EVENT: Rain flooding parts of the nest!\n";
                    // Workers repair
                    for (auto& ant : colony) {
                        if (ant->getRole() == AntRole::WORKER) {
                            ant->buildNest();
                        }
                    }
                    break;
            }
            printSeparator('*');
        }
        
        // End of iteration
        iteration++;
        printSeparator();
        std::cout << "\nPress Enter to continue to next iteration, or Ctrl+C to exit...";
        std::cin.get();
    }
    
    // End of simulation
    clearScreen();
    std::cout << "=== End of Ant Colony Simulation ===\n\n";
    std::cout << "Final colony status:\n";
    
    int alive = 0, dead = 0;
    for (const auto& ant : colony) {
        if (ant->isAlive()) {
            alive++;
        } else {
            dead++;
        }
    }
    
    std::cout << "Colony size: " << colony.size() << " ants\n";
    std::cout << "Alive: " << alive << " ants\n";
    std::cout << "Dead: " << dead << " ants\n\n";
    
    // Ask user if they want to see detailed status of all ants
    std::cout << "Do you want to see detailed status of all ants? (y/n): ";
    char choice;
    std::cin >> choice;
    
    if (choice == 'y' || choice == 'Y') {
        for (const auto& ant : colony) {
            ant->displayStatus();
            std::cout << std::endl;
        }
    }
    
    std::cout << "\nThank you for running the Ant Colony Simulator!\n";
    return 0;
}