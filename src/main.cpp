// main.cpp - Ant Colony Simulator with main simulation loop
#include "Timer.h"
#include "Visualizer.h"
#include "World.h"
#include <vector>

// Simulation parameters
const int initialColonySize = 10;
const std::pair<unsigned int, unsigned int> worldSize = {50, 40};
const std::pair<unsigned int, unsigned int> screenSize = {800, 600};
const float simulationStepsPerSecond = 0.2;

int main() {
    Timer timer(simulationStepsPerSecond);
    World world(worldSize.first, worldSize.second, initialColonySize);
    world.initialize(initialColonySize);
    Visualizer visualizer(worldSize, screenSize);
    bool running = true;
    
    while (running && visualizer.isOpen()) {
        visualizer.clear();
        visualizer.processEvents();
        // Get frame time for FPS calculation
        float frameTime = timer.getFrameDeltaTime();
        
        // Run simulation steps as needed
        int stepsToRun = timer.getSimulationStepsToRun();
        for (int i = 0; i < stepsToRun; i++) {
            world.update();
        }
        float accumulatedTime = timer.getAccumulatedTime();
        float interpolation = accumulatedTime / timer.getSimulationStepSize();
        visualizer.drawWorld(world, interpolation);
        // Display simulation stats
        float fps = 1.0f / frameTime;
        visualizer.displayStats(fps, stepsToRun);
        visualizer.display();
    }
    return 0;
}