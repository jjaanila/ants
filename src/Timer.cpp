#include <chrono>

#include "Timer.h"

// Time since last frame (for FPS calculations)
float Timer::getFrameDeltaTime() {
    TimePoint now = Clock::now();
    Duration frameTime = now - lastFrameTime;
    lastFrameTime = now;
    return frameTime.count();
}

// How many simulation steps should run now
int Timer::getSimulationStepsToRun() {
    TimePoint now = Clock::now();
    Duration elapsed = now - lastSimulationUpdate;
    float elapsedSeconds = elapsed.count();
    accumulatedTime += elapsedSeconds;
    int steps = static_cast<int>(accumulatedTime / simulationStepSize);
    
    if (steps > 0) {
        // Calculate exact time to advance
        accumulatedTime -= steps * simulationStepSize;
        
        // Add the duration to the last update time using the + operator
        lastSimulationUpdate = now;
    }
    
    // Limit max steps to prevent spiral of death
    return std::min(steps, 5);
}

float Timer::getAccumulatedTime() const { 
    return accumulatedTime; 
}

float Timer::getSimulationStepSize() const {
    return simulationStepSize;
}