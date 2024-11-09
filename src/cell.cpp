#include "cell.hpp"
#include "grid.hpp"
#include <SFML/System/Vector2.hpp>

Cell::Cell() : type(CellType::none), p(0, 0) {
}

Cell::Cell(CellType type) : type(type), p(0, 0) {
}

Cell::Cell(int x, int y) : p(x, y) {
}

Cell::Cell(CellType type, int x, int y) : type(type), p(x, y) {
}

sf::Vector2i Cell::nextPosition(Grid &grid, float dt) {
    // Immovable types.
    if (type == CellType::none) return p;
    if (type == CellType::wood) return p;

    if (type == CellType::sand) {
        if (p.y > 0) {
            sf::Vector2i lookAhead {0, -1};
            Cell testCell {grid.getCell(p + lookAhead)};
            if (testCell.type == CellType::none) {
                return p + lookAhead;
            }
        }
    }

    return p;
}