#include "grid.hpp"

Grid::Grid(int width, int height) : width(width), height(height) {
    grid.resize(width * height);
    for (int i = 0; i < grid.size() - 1; i++) {
        Cell newCell {CellType::none};
        newCell.y = i / width;
        newCell.x = i % width;
        grid[i] = newCell;
    }
}

Cell& Grid::getCell(int index) {
    return grid[index];
}

Cell& Grid::getCell(int x, int y) {
    return grid[x + y * width];
}

void Grid::setCell(int index, CellType type) {
    grid[index].type = type;
}

void Grid::setCell(int x, int y, CellType type) {
    grid[x + y * width].type = type;
}

size_t Grid::size() {
    return grid.size();
}