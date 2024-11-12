#include "cell.hpp"
#include "grid.hpp"
#include <SFML/System/Vector2.hpp>

Cell::Cell() : type(CellType::air), p(0, 0), active(false) {
}

Cell::Cell(CellType type) : type(type), p(0, 0), active(false) {
}

Cell::Cell(int x, int y) : p(x, y), active(false) {
}

Cell::Cell(CellType type, int x, int y) : type(type), p(x, y), active(false) {
}