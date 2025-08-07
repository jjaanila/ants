// main.cpp - Ant Colony Simulator with main simulation loop
#include "Timer.h"
#include "Visualizer.h"
#include "World.h"
#include <vector>

// Simulation parameters
const int INITIAL_COLONY_SIZE = 10;
const unsigned int WORLD_WIDTH = 800;
const unsigned int WORLD_HEIGHT = 600;
const float SIMULATION_STEPS_PER_SECOND = 0.2;

int main() {
    Timer timer(SIMULATION_STEPS_PER_SECOND);
    World world(WORLD_WIDTH, WORLD_HEIGHT, INITIAL_COLONY_SIZE);
    world.initialize(INITIAL_COLONY_SIZE);
    Visualizer visualizer(world.getWidth(), world.getHeight());
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