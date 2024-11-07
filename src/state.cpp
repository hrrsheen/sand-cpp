#include "state.hpp"

State::State(int width, int height) : 
    brushType(CellType::sand),
    grid(width, height) {
}

void State::draw(Screen &screen) {
    screen.clear();
    for (int i = 0; i < grid.size() - 1; i++) {
        Cell &cell {grid.getCell(i)};
        if (cell.type != CellType::none) {
            sf::RectangleShape cellRect;
            cellRect.setSize(sf::Vector2f{1.f, 1.f});
            cellRect.setOrigin(0.f, -1.f);
            cellRect.setPosition(cell.x, cell.y);
            cellRect.setFillColor(sf::Color::Cyan);

            screen.draw(cellRect);
        }
    }
    screen.display();
}

