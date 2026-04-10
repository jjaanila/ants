#pragma once

class UniqueIdGenerator {
private:
    int nextId = 0;

public:
    int getNextId();
};
