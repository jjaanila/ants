#include <SFML/Graphics.hpp>
#include <vector>
#include "Position.h"
#include "Visualizer.h"
#include "World.h"


Visualizer::Visualizer(unsigned int width, unsigned int height) : 
    window(sf::VideoMode({width, height}), "Ants") {
    window.setFramerateLimit(144);
}

void Visualizer::processEvents() {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>())
            window.close();
    }
}

void Visualizer::clear() {
    window.clear(sf::Color(100, 100, 100));
    antShapes.clear();
    foodShapes.clear();
}

void Visualizer::drawNest(float x, float y, float radius) {
    nestShape.setSize(sf::Vector2f(radius * 2.0f, radius * 2.0f));
    nestShape.setPosition({x - radius, y - radius});
    nestShape.setFillColor(sf::Color(139, 69, 19)); // Brown
    window.draw(nestShape);
}

void Visualizer::drawAnt(Ant& ant) {
    sf::RectangleShape antShape;
    antShape.setSize(sf::Vector2f(ant.getSize(), ant.getSize()));
    const auto position = ant.getPosition();
    antShape.setPosition({position.getX(), position.getY()});
    switch(ant.getRole()) {
        case AntRole::QUEEN:
            antShape.setFillColor(sf::Color::Red);
            break;
        case AntRole::NURSE:
            antShape.setFillColor(sf::Color::Magenta);
            break;
        case AntRole::FORAGER:
            antShape.setFillColor(sf::Color::Blue);
            break;
        case AntRole::SOLDIER:
            antShape.setFillColor(sf::Color::Yellow);
            break;
        default:
            antShape.setFillColor(sf::Color::Black);
    }
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

void Visualizer::drawWorldInterpolated(const World& world, float interpolation) {
    // Draw entities with interpolation between their current and previous positions
    for (const auto& ant : world.getAnts()) {
        FloatPosition currentPos = (*ant).getPosition();
        FloatPosition prevPos = (*ant).getPreviousPosition();
        
        // Interpolate between previous and current position
        float x = prevPos.getX() + (currentPos.getX() - prevPos.getX()) * interpolation;
        float y = prevPos.getY() + (currentPos.getY() - prevPos.getY()) * interpolation;
        
        drawAnt(*ant);
    }
    
    // Draw other world elements
    // ...
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