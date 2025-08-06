#pragma once

#include <string>
#include <cmath>
#include "Vector2D.h"

// Forward declarations
class IntegerPosition;
class FloatPosition;

/**
 * @brief Base position class with common functionality
 */
class Position {
public:
    virtual ~Position() = default;
    
    // Common interface for all position types
    virtual float getX() const = 0;
    virtual float getY() const = 0;
    
    // Shared implementation for distance calculation
    template<typename PositionType>
    float calculateDistance(const PositionType& other) const {
        float dx = getX() - other.getX();
        float dy = getY() - other.getY();
        return std::sqrt(dx * dx + dy * dy);
    }
    
    template<typename PositionType>
    float calculateSquaredDistance(const PositionType& other) const {
        float dx = getX() - other.getX();
        float dy = getY() - other.getY();
        return dx * dx + dy * dy;
    }
    
    // String representation
    virtual std::string toString() const {
        return "(" + std::to_string(getX()) + "," + std::to_string(getY()) + ")";
    }
};

/**
 * @brief Represents a position in the world using integer X,Y coordinates
 */
class IntegerPosition : public Position {
private:
    int x;
    int y;

public:
    IntegerPosition(int x = 0, int y = 0) : x(x), y(y) {}
    
    // Implementation of the Position interface
    float getX() const override { return static_cast<float>(x); }
    float getY() const override { return static_cast<float>(y); }
    
    // Integer-specific getters
    int getIntX() const { return x; }
    int getIntY() const { return y; }
    
    // Distance methods using the base class implementation
    float distanceTo(const IntegerPosition& other) const {
        return calculateDistance(other);
    }
    
    float distanceTo(const FloatPosition& other) const {
        return calculateDistance(other);
    }
    
    float squaredDistanceTo(const IntegerPosition& other) const {
        return calculateSquaredDistance(other);
    }
    
    float squaredDistanceTo(const FloatPosition& other) const {
        return calculateSquaredDistance(other);
    }
    
    // Assignment operator
    IntegerPosition& operator=(const IntegerPosition& other) {
        if (this != &other) {
            x = other.x;
            y = other.y;
        }
        return *this;
    }
    
    // Equality operator for comparison
    bool operator==(const IntegerPosition& other) const {
        return x == other.x && y == other.y;
    }
    
    // Addition operator for IntegerPosition
    IntegerPosition operator+(const IntegerPosition& other) const {
        return IntegerPosition(x + other.x, y + other.y);
    }
    
    // Addition operator with FloatPosition (returns FloatPosition)
    FloatPosition operator+(const FloatPosition& other) const;
    
    // Subtraction operator for IntegerPosition
    IntegerPosition operator-(const IntegerPosition& other) const {
        return IntegerPosition(x - other.x, y - other.y);
    }
    
    // Subtraction operator with FloatPosition (returns FloatPosition)
    FloatPosition operator-(const FloatPosition& other) const;
    
    // Custom string representation
    std::string toString() const override {
        return "Int" + Position::toString();
    }
};

/**
 * @brief Represents a position in the world using float X,Y coordinates
 */
class FloatPosition : public Position {
private:
    Vector2D vector;
    
public:
    FloatPosition() : vector() {}
    FloatPosition(float x, float y) : vector(x, y) {}
    FloatPosition(const Vector2D& v) : vector(v) {}
    FloatPosition(const IntegerPosition& pos) : vector(pos.getX(), pos.getY()) {}
    
    // Implementation of the Position interface
    float getX() const override { return vector.x; }
    float getY() const override { return vector.y; }
    
    // Get the underlying Vector2D
    Vector2D toVector2D() const { return vector; }
    
    // Distance methods using the base class implementation
    float distanceTo(const FloatPosition& other) const {
        return calculateDistance(other);
    }
    
    float distanceTo(const IntegerPosition& other) const {
        return calculateDistance(other);
    }
    
    float squaredDistanceTo(const FloatPosition& other) const {
        return calculateSquaredDistance(other);
    }
    
    float squaredDistanceTo(const IntegerPosition& other) const {
        return calculateSquaredDistance(other);
    }
    
    // Assignment operator
    FloatPosition& operator=(const FloatPosition& other) {
        if (this != &other) {
            vector = other.vector;
        }
        return *this;
    }

    // Equality operator
    bool operator==(const FloatPosition& other) const {
        return vector.x == other.vector.x && vector.y == other.vector.y;
    }
    
    bool operator==(const IntegerPosition& other) const {
        return vector.x == other.getX() && vector.y == other.getY();
    }
    
    // Addition operator for FloatPosition
    FloatPosition operator+(const FloatPosition& other) const {
        return FloatPosition(vector.x + other.vector.x, vector.y + other.vector.y);
    }
    
    // Addition operator with IntegerPosition
    FloatPosition operator+(const IntegerPosition& other) const {
        return FloatPosition(vector.x + other.getX(), vector.y + other.getY());
    }
    
    // Addition with Vector2D
    FloatPosition operator+(const Vector2D& v) const {
        return FloatPosition(vector.x + v.x, vector.y + v.y);
    }
    
    // Subtraction operator for FloatPosition
    FloatPosition operator-(const FloatPosition& other) const {
        return FloatPosition(vector.x - other.vector.x, vector.y - other.vector.y);
    }
    
    // Subtraction operator with IntegerPosition
    FloatPosition operator-(const IntegerPosition& other) const {
        return FloatPosition(vector.x - other.getX(), vector.y - other.getY());
    }
    
    // Subtraction that returns Vector2D
    Vector2D toVector(const FloatPosition& other) const {
        return Vector2D(vector.x - other.vector.x, vector.y - other.vector.y);
    }

    // Custom string representation
    std::string toString() const override {
        return "Float" + Position::toString();
    }
};

inline FloatPosition IntegerPosition::operator+(const FloatPosition& other) const {
    return FloatPosition(x + other.getX(), y + other.getY());
}

inline FloatPosition IntegerPosition::operator-(const FloatPosition& other) const {
    return FloatPosition(x - other.getX(), y - other.getY());
}
