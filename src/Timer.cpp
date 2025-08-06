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
    
    int steps = static_cast<int>(elapsedSeconds / simulationStepSize);
    
    if (steps > 0) {
        // Calculate exact time to advance
        float timeToAdvanceSeconds = steps * simulationStepSize;
        
        // Create a duration using the same clock's duration representation
        auto timeToAdvance = std::chrono::duration_cast<Clock::duration>(
            std::chrono::duration<float>(timeToAdvanceSeconds)
        );
        
        // Add the duration to the last update time using the + operator
        lastSimulationUpdate = lastSimulationUpdate + timeToAdvance;
    }
    
    // Limit max steps to prevent spiral of death
    return std::min(steps, 5);
}