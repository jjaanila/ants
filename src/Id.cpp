#include "Id.h"

// Initialize the static counter
int UniqueIdGenerator::nextId = 0;

// Implement the getNextId method
int UniqueIdGenerator::getNextId() {
    return nextId++;
}