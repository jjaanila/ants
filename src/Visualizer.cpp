#include <SFML/Graphics.hpp>
#include <vector>
#include "Visualizer.h"


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

void Visualizer::draw(sf::RectangleShape shape) {
    window.draw(shape);
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

void Visualizer::display() {
    window.display();
}

bool Visualizer::isOpen() const {
    return window.isOpen();
}