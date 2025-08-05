#ifndef ID_H
#define ID_H

/**
 * @brief A simple utility class for generating unique sequential IDs
 */
class UniqueIdGenerator {
private:
    /**
     * @brief The next ID to be assigned
     */
    static int nextId;
    
public:
    /**
     * @brief Get the next available unique ID
     * @return int A unique integer value
     */
    static int getNextId();
};

#endif // ID_H