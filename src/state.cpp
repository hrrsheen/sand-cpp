#include "state.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

State::State(int width, int height) : 
    brushType(CellType::sand),
    grid(width, height) {
}

void State::step(float dt) {
    for (int y = 0; y < grid.height - 1; y++) {
        // Alternate processing rows left to right and right to left.
        const int startEnd[2] = {0, grid.width - 1};
        int direction {y % 2 ? -1 : 1};

        // Process each row.
        for (int x = startEnd[y % 2]; x != startEnd[(y + 1) % 2] + direction; x += direction) {
            Cell &current {grid.getCell(x, y)};
            applyRules(current);
        }
    }
}

void State::draw(Screen &screen) {
    screen.clear();
    for (int i = 0; i < grid.size() - 1; i++) {
        Cell &cell {grid.getCell(i)};
        if (cell.type != CellType::air) {
            screen.addCell(sf::Vector2f(cell.p), 1.f, sf::Color::Cyan);
        } else {
            // vertices[i].color = sf::Color::Black;
        }
    }
    screen.draw();
    screen.display();
}


void State::applyRules(Cell &cell) {
    // Immovable types.
    if (cell.type == CellType::air) return;
    if (cell.type == CellType::wood) return;

    if (cell.type == CellType::sand) {
        if (cell.p.y > 0) {
            sf::Vector2i lookAhead {0, -1};
            Cell &testCell {grid.getCell(cell.p + lookAhead)};
            if (testCell.type == CellType::air) {
                testCell.type = cell.type;
                cell.type = CellType::air;

                cell.active = true;
                testCell.active = true;
            }
        }
    }
}

