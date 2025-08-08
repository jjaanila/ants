#include <SFML/Graphics.hpp>
#include <vector>
#include "Position.h"
#include "Visualizer.h"
#include "World.h"

Visualizer::Visualizer(
    std::pair<unsigned int, unsigned int> worldSize,
    std::pair<unsigned int, unsigned int> screenSize
)
    : worldSize(worldSize),
      window(sf::VideoMode({screenSize.first, screenSize.second}), "Ants") {
    window.setFramerateLimit(144);
}

float Visualizer::getWorldToScreenMultiplier() {
    return (static_cast<float>(window.getSize().y - (marginWidth * 2)) / worldSize.second);
}

float Visualizer::scaleToScreen(float worldValue) {
    return worldValue * getWorldToScreenMultiplier();
}

float Visualizer::scaleToWorld(float worldValue) {
    return worldValue / getWorldToScreenMultiplier();
}

float Visualizer::toScreenCoordinate(float worldCoordinate) {
    return scaleToScreen(worldCoordinate) + marginWidth;
}

float Visualizer::toWorldCoordinate(float screenCoordinate) {
    return scaleToWorld(screenCoordinate - marginWidth);
}

void Visualizer::processEvents() {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>())
            window.close();
    }
}

void Visualizer::clear() {
    window.clear(backgroundColor);
    antShapes.clear();
    foodShapes.clear();
}

void Visualizer::drawNest(World& world) {
    sf::RectangleShape nestShape;
    const auto position = world.getNestEntrancePosition();
    nestShape.setSize(sf::Vector2f());
    nestShape.setPosition({toScreenCoordinate(position.getX()), toScreenCoordinate(position.getY())});
    nestShape.setFillColor(nestColor);
    window.draw(nestShape);
}

void Visualizer::drawTile(const Tile* tile) {
    if (!tile) return;
    const auto tileWorldPosition = tile->getPosition();
    float screenX = toScreenCoordinate(tileWorldPosition.getX());
    float screenY = toScreenCoordinate(tileWorldPosition.getY());
    const float tileSize = scaleToScreen(1);
    
    // Create a rectangle shape for the tile
    sf::RectangleShape tileShape(sf::Vector2f(tileSize, tileSize));
    tileShape.setPosition({screenX, screenY});
    
    switch (tile->getTerrain()) {
    case TerrainType::GRASS:
        tileShape.setFillColor(sf::Color(34, 139, 34));  // Forest Green
        break;
    case TerrainType::ROCK:
        tileShape.setFillColor(sf::Color(128, 128, 128));  // Gray
        break;
    case TerrainType::SAND:
        tileShape.setFillColor(sf::Color(238, 214, 175));  // Sandy Brown
        break;
    case TerrainType::SOIL:
        tileShape.setFillColor(sf::Color(200, 200, 150));  // Soil brown
        break;
    default:
        tileShape.setFillColor(sf::Color(169, 169, 169));  // Dark Gray (default)
        break;
    }
    
    tileShape.setOutlineThickness(1.0f);
    tileShape.setOutlineColor(sf::Color(0, 0, 0, 40));  // Semi-transparent black
    window.draw(tileShape);
}

void Visualizer::drawTerrain(World& world) {
    world.forEachTile([this](Tile* tile) {
        drawTile(tile);
    });
}

void Visualizer::drawAnt(Ant& ant, float interpolation) {
    sf::RectangleShape antShape;
    antShape.setSize(sf::Vector2f(scaleToScreen(ant.getSize()), scaleToScreen(ant.getSize())));
    FloatPosition currentPos = ant.getPosition();
    FloatPosition prevPos = ant.getPreviousPosition();
    
    // Interpolate between previous and current position
    float x = prevPos.getX() + (currentPos.getX() - prevPos.getX()) * interpolation;
    float y = prevPos.getY() + (currentPos.getY() - prevPos.getY()) * interpolation;
    antShape.setPosition({toScreenCoordinate(x), toScreenCoordinate(y)});
    antShape.setFillColor(ant.getColor());
    window.draw(antShape);
}

void Visualizer::drawFood(float x, float y, float amount) {
    sf::RectangleShape foodShape;
    foodShape.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
    foodShape.setPosition({x, y});
    
    // Brighter green for more food
    int intensity = std::min(255, static_cast<int>(100 + amount * 155 / 100));
    foodShape.setFillColor(sf::Color(0, intensity, 0));
    
    foodShapes.push_back(foodShape);
    window.draw(foodShape);
}

void Visualizer::drawWorld(World& world, float interpolation) {
    drawTerrain(world);
    for (const auto& ant : world.getAnts()) {
        drawAnt(*ant, interpolation);
    }
}

void Visualizer::display() {
    window.display();
}

void Visualizer::displayStats(float fps, int simStepsLastFrame) {
    sf::Font font("resources/Arial Unicode.ttf");
    sf::Text fpsText(font);
    fpsText.setCharacterSize(12);
    fpsText.setFillColor(sf::Color::White);
    
    std::string statsStr = "FPS: " + std::to_string(static_cast<int>(fps)) + 
                          " | Sim Steps: " + std::to_string(simStepsLastFrame);
    fpsText.setString(statsStr);
    
    window.draw(fpsText);
}

bool Visualizer::isOpen() const {
    return window.isOpen();
}