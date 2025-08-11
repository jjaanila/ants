#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include "Ant.h"
#include "Tile.h"

class World;
class Vector2D;
enum class AntRole;

const sf::Color backgroundColor = sf::Color(100, 100, 100);
const sf::Color nestColor = sf::Color(139, 69, 19);

class Visualizer {
private:
    sf::RenderWindow window;
    std::pair<unsigned int, unsigned int> worldSize;
    std::vector<sf::RectangleShape> antShapes;
    sf::RectangleShape nestShape;
    std::vector<sf::RectangleShape> foodShapes;
    float marginWidth = 20.0f; 
    float getWorldToScreenMultiplier();
    float scaleToScreen(float worldValue);
    float scaleToWorld(float screenValue);
    float toScreenCoordinate(float worldValue);
    float toWorldCoordinate(float screenValue);

    void drawNest(World& world);
    
    void drawTerrain(World& world);

    void drawTile(const Tile* tile);
    
    void drawAnt(Ant& ant, float interpolation);
    
    void drawFood(const IntegerPosition& pos, float amount);

public:
    Visualizer(std::pair<unsigned int, unsigned int> worldSize, std::pair<unsigned int, unsigned int> screenSize);

    void processEvents();

    void clear();

    void drawWorld(World& world, float interpolation);
    
    void display();

    void displayStats(float fps, int simStepsLastFrame);
    
    bool isOpen() const;
};