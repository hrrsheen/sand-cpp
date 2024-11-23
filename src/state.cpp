#include "helpers.hpp"
#include "state.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

State::State(int width, int height) : 
    brush{1, 5}, // TODO: make more general.
    grid(width, height) {
    std::vector<std::string> els {"air", "sand", "stone"}; // TODO: Remove this, it's debugging code. Use a file instead
    grid.initProperties(els);
    grid.initCells();
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
        if (cell.redraw) {
            screen.updateCell(cell, i);
            cell.redraw = false;
        }
    }
    screen.draw();
    screen.display();
}


void State::applyRules(Cell &cell) {
    ElementProperties &properties {grid.getProperties(cell)};
    // Immovable types.
    if (properties.type == ElementType::null
        || properties.type == ElementType::air 
        || properties.type == ElementType::solidImmovable) {
        return;
    }
    
    if (properties.type == ElementType::solidMovable) {
        applySolidRules(cell, properties);
    } else if (properties.type == ElementType::liquid) {
        applyLiquidRules(cell, properties);
    } else if (properties.type == ElementType::gas) {
        applyGasRules(cell, properties);
    }
}

void State::applySolidRules(Cell &cell, ElementProperties &properties) {
    if (cell.p.y > 0) {
        sf::Vector2i lookAhead {0, -1};
        Cell *testCell {&grid.getCell(cell.p + lookAhead)};
        ElementProperties *testProperties {&grid.getProperties(*testCell)};
        if (properties.canDisplace(*testProperties)) {
            swap(&cell, testCell);

            cell.redraw = true;
            testCell->redraw = true;
        } else {
            int randDir {quickRandInt(2)};
            randDir = 2 * randDir - 1; // Translate to +/- 1.
            lookAhead.x = randDir;
            testCell = &grid.getCell(cell.p + lookAhead);
            testProperties = &grid.getProperties(*testCell);
            if (properties.canDisplace(*testProperties)) {
                swap(&cell, testCell);

                cell.redraw = true;
                testCell->redraw = true;
            }
        }
    }
}

void State::applyLiquidRules(Cell &cell, ElementProperties &properties) {
    
}

void State::applyGasRules(Cell &cell, ElementProperties &properties) {
    
}
