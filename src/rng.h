#pragma once

#include <random>
#include <chrono>

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