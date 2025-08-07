#pragma once

#include <chrono>

/**
 * @brief Timer class for managing simulation and rendering timing
 * 
 * This class helps decouple simulation updates from rendering frames,
 * allowing fixed-time simulation steps while rendering at maximum speed.
 */
class Timer {
private:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;
    using Duration = std::chrono::duration<float>;
    
    TimePoint lastSimulationUpdate;  // Time of last simulation update
    TimePoint lastFrameTime;         // Time of last frame render
    float simulationStepSize;        // Fixed timestep size in seconds
    float accumulatedTime = 0.0f;
    
public:
    /**
     * @brief Construct a new Timer object
     * 
     * @param simulationStepsPerSecond How many simulation steps per second (default: 10)
     */
    Timer(float simulationStepsPerSecond = 10.0f) : simulationStepSize(1.0f / simulationStepsPerSecond) {}
    
    /**
     * @brief Get time elapsed since last frame
     * 
     * @return float Time in seconds since last call to this method
     */
    float getFrameDeltaTime();
    
    /**
     * @brief Calculate how many simulation steps should be run
     * 
     * @return int Number of simulation steps to run (capped at 5)
     */
    int getSimulationStepsToRun();

    float getAccumulatedTime() const;
    float getSimulationStepSize() const;
};