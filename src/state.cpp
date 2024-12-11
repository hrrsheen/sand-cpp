#include "helpers.hpp"
#include "state.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

State::State(int width, int height) : 
    brush{Element::air, 5},
    grid(width, height),
    chunks(10, 10, 50) {
    grid.initProperties();
    grid.initCells();
}

void State::step(float dt) {
    Lerp lerp(sf::Vector2i(19, 10), sf::Vector2i(0, 0));
    for (sf::Vector2i v : lerp) {
        std::cout << "(" << v.x << ", " << v.y << ")\n";
    }
    for (int y = 0; y < grid.height; y++) {
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
        if (cell.redraw) {
            screen.updateCell(cell, i);
            cell.redraw = false;
        }
    }
    screen.draw();
    screen.display();
}


void State::applyRules(Cell &cell) {
    ElementProperties &properties {cell.properties()};
    // Immovable types.
    if (!properties.isMobile()) return;
    
    if (properties.type == ElementType::solidMovable) {
        applySolidRules(cell, properties);
    } else if (properties.type == ElementType::liquid) {
        applyLiquidRules(cell, properties);
    } else if (properties.type == ElementType::gas) {
        applyGasRules(cell, properties);
    }
}

void State::applySolidRules(Cell &cell, ElementProperties &properties) {
    // Cells flagged for redraw have already been simulated for this step.
    if (!cell.active) return;

    if (cell.p.y > 0) {
        sf::Vector2i lookAhead {0, -1};
        // Build the sequence of directions to check for free space.
        int randDir {quickRandInt(2)};
        randDir = 2 * randDir - 1; // Translate to +/- 1.
        std::vector<int> directions {0, randDir, -randDir};

        for (int dir : directions) {
            lookAhead.x = dir;
            Cell *testCell {&grid.getCell(cell.p + lookAhead)};
            ElementProperties &testProperties {testCell->properties()};
            if (properties.canDisplace(testProperties)) {
                swap(&cell, testCell);

                cell.redraw = true;
                testCell->redraw = true;
                return;
            }
        }

        // cell.active = false;
    }
}

void State::applyLiquidRules(Cell &cell, ElementProperties &properties) {
    if (!cell.active || cell.redraw) return;

    if (cell.p.y > 0) {
        sf::Vector2i lookAhead {0, -1};
        // Build the sequence of directions to check for free space.
        int randDir {quickRandInt(2)};
        randDir = 2 * randDir - 1; // Translate to +/- 1.
        std::vector<int> directions {0, randDir, -randDir};

        
    }
}

void State::applyGasRules(Cell &cell, ElementProperties &properties) {
    
}
