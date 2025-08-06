#pragma once

#include <cmath>

class Vector2D {
public:
    float x;
    float y;

    // Constructors
    Vector2D() : x(0.0f), y(0.0f) {}
    Vector2D(float x, float y) : x(x), y(y) {}

    // Vector operations
    Vector2D operator+(const Vector2D& other) const {
        return Vector2D(x + other.x, y + other.y);
    }

    Vector2D operator-(const Vector2D& other) const {
        return Vector2D(x - other.x, y - other.y);
    }

    Vector2D operator*(float scalar) const {
        return Vector2D(x * scalar, y * scalar);
    }

    Vector2D operator-() const {
        return Vector2D(-x, -y);
    }

    // Utility methods
    float magnitude() const {
        return std::sqrt(x*x + y*y);
    }

    Vector2D normalized() const {
        float mag = magnitude();
        if (mag < 0.00001f) return Vector2D(0, 0);
        return Vector2D(x / mag, y / mag);
    }

    float dot(const Vector2D& other) const {
        return x * other.x + y * other.y;
    }
};

// Non-member operator
inline Vector2D operator*(float scalar, const Vector2D& vector) {
    return vector * scalar;
}