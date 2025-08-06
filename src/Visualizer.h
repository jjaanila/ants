#pragma once

#include <vector>
#include <SFML/Graphics.hpp>

// Forward declarations if needed
class World;
class Vector2D;
enum class AntRole;

class Visualizer {
private:
    sf::RenderWindow window;
    std::vector<sf::RectangleShape> antShapes;
    sf::RectangleShape nestShape;
    std::vector<sf::RectangleShape> foodShapes;
    
    const float CELL_SIZE = 10.0f; // Size of each cell/ant
    
public:
    // Constructor to initialize the window
    Visualizer(int width, int height);
    
    // Process window events like close button clicks
    void processEvents();
    
    // Clear the window and prepare for new frame
    void clear();
    
    // Draw the nest at a specific position with a radius
    void drawNest(float x, float y, float radius);
    
    // Draw an ant with position and role
    void draw(sf::RectangleShape shape);
    
    // Draw food at a position with an amount (affects color intensity)
    void drawFood(float x, float y, float amount);
    
    // Finish drawing and display the frame
    void display();
    
    // Check if the window is still open
    bool isOpen() const;
};