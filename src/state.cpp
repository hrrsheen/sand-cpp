#include "state.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

State::State(int width, int height) : 
    brushType(CellType::sand),
    grid(width, height),
    vertices(sf::Points, width * height) {
}

void State::tick(float dt) {
    grid.evolve(dt);
}

void State::draw(Screen &screen) {
    screen.clear();
    for (int i = 0; i < grid.size() - 1; i++) {
        Cell &cell {grid.getCell(i)};
        vertices[i].position = sf::Vector2f(cell.p);
        if (cell.type != CellType::none) {
            vertices[i].color = sf::Color::Cyan;
        //     sf::RectangleShape cellRect;
        //     cellRect.setSize(sf::Vector2f{1.f, 1.f});
        //     cellRect.setOrigin(0.f, -1.f);
        //     cellRect.setPosition(cell.p.x, cell.p.y);
        //     cellRect.setFillColor(sf::Color::Cyan);

        //     screen.draw(cellRect);
        } else {
            vertices[i].color = sf::Color::Black;
        }
    }
    screen.draw(vertices);
    screen.display();
}

