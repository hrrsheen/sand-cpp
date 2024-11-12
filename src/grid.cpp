#include "grid.hpp"
#include <algorithm>

Grid::Grid(int width, int height) : width(width), height(height) {
    grid.resize(width * height);
    for (int i = 0; i < grid.size() - 1; i++) {
        Cell newCell {CellType::air, i % width, i / width};
        grid[i] = newCell;
    }
}

Cell& Grid::getCell(int index) {
    return grid.at(index);
}

Cell& Grid::getCell(int x, int y) {
    return grid.at(x + y * width);
}

Cell& Grid::getCell(sf::Vector2i p) {
    return grid.at(p.x + p.y * width);
}

void Grid::setCell(int index, CellType type) {
    grid.at(index).type = type;
}

void Grid::setCell(int x, int y, CellType type) {
    grid.at(x + y * width).type = type;
}

size_t Grid::size() const {
    return grid.size();
}

int Grid::coordsToIndex(int x, int y) {
    return x + y * width;
}
