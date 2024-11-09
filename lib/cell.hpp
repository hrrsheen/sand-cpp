#ifndef CELL_HPP
#define CELL_HPP

#include <cstdint>
#include <SFML/System/Vector2.hpp>

class Grid;

enum CellType : uint8_t {
    none,
    sand,
    water,
    wood
};

class Cell {
public:
    Cell();
    Cell(CellType type);
    Cell(int x, int y);
    Cell(CellType type, int x, int y);

    sf::Vector2i nextPosition(Grid &grid, float dt);

    CellType type;
    sf::Vector2i p; // The (x, y) coordinate of this cell within its grid.
};

#endif