// main.cpp - Ant Colony Simulator with main simulation loop
#include "Ant.h"
#include "World.h"
#include "Visualizer.h"
#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <chrono>
#include <random>
#include <cstdlib>
#include <ctime>
#include <iomanip>

// Random number generator
std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));

// Return random int in range [min, max]
int getRandomInt(int min, int max) {
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(rng);
}

// Return random float in range [min, max]
float getRandomFloat(float min, float max) {
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(rng);
}

// Simulation parameters
const int INITIAL_COLONY_SIZE = 20;
const int DELAY_MS = 500;           // Delay between ticks (milliseconds)

int main() {
    World world(40, 20);
    world.initialize();
    Visualizer visualizer(world.getWidth(), world.getHeight());

    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    std::vector<std::shared_ptr<Ant>> colony;
    const IntegerPosition nestPosition = world.getNestEntrancePosition();
    colony.push_back(std::make_shared<Ant>(AntRole::QUEEN, 8.0, "dark red"));
    colony.back()->setColony("Fire Nation");
    world.addAnt(colony.back(), nestPosition);
    for (int i = 1; i < INITIAL_COLONY_SIZE; ++i) {
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
        float size = getRandomFloat(4.0, 7.0);  // Size in mm
        std::string color;
        switch (role) {
            case AntRole::WORKER: color = "red"; break;
            case AntRole::FORAGER: color = "light red"; break;
            case AntRole::SOLDIER: color = "dark red"; break;
            case AntRole::NURSE: color = "orange"; break;
            case AntRole::DRONE: color = "black"; break;
            default: color = "brown"; break;
        }
        
        colony.push_back(std::make_shared<Ant>(role, size, color));
        colony.back()->setColony("Fire Nation");
        world.addAnt(colony.back(), nestPosition);
    }
    bool running = true;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    while (running) {
        for (auto& ant : colony) {
        }
    }
    
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