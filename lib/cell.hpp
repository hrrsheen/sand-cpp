#ifndef CELL_HPP
#define CELL_HPP

#include <cstdint>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

class Grid;

enum CellType : uint8_t {
    air,
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

    void assign(CellType type);

    friend void swap(Cell &, Cell &);

    CellType type;
    sf::Vector2i p; // The (x, y) coordinate of this cell within its grid.
    sf::Color colour; // The colour of this cell.

    bool redraw; // Tracks whether this cell has changes since the previous frame.

private:
    sf::Color getPalette(); // Returns a colour from the palette for this instance's cell type.
};

void swap(Cell &cellA, Cell &cellB);

#endif