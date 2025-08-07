#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include "Ant.h"

class World;
class Vector2D;
enum class AntRole;

class Visualizer {
private:
    sf::RenderWindow window;
    std::vector<sf::RectangleShape> antShapes;
    sf::RectangleShape nestShape;
    std::vector<sf::RectangleShape> foodShapes;
    
    const float CELL_SIZE = 10.0f;     
public:
    Visualizer(unsigned int width, unsigned int height);

    void processEvents();

    void clear();

    void drawNest(float x, float y, float radius);
    
    void drawAnt(Ant& ant, float interpolation);
    
    void drawFood(float x, float y, float amount);

    void drawWorld(const World& world, float interpolation);
    
    void display();

    void displayStats(float fps, int simStepsLastFrame);
    
    bool isOpen() const;
};