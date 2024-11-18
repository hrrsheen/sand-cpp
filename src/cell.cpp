#include "cell.hpp"
#include "grid.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

Cell::Cell() : type(CellType::air), p(0, 0), redraw(false) {
}

Cell::Cell(CellType type) : type(type), p(0, 0), redraw(false) {
}

Cell::Cell(int x, int y) : p(x, y), redraw(false) {
}

Cell::Cell(CellType type, int x, int y) : type(type), p(x, y), redraw(false) {
}

void Cell::assign(CellType type) {
    Cell::type = type;
    colour = getPalette();
    redraw = true;
}

sf::Color Cell::getPalette() {
    if (type == CellType::air) {
        return sf::Color(0x00000000);
    } else if (type == CellType::sand) {
        return sf::Color(0xfabf73ff);
    }   
}

void swap(Cell &cellA, Cell &cellB) {
    Cell tmpCell = cellA;
    cellA.type = cellB.type;
    cellA.colour = cellB.colour;
    
    cellB.type = tmpCell.type;
    cellB.colour = tmpCell.colour;
}