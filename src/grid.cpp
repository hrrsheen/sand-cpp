#include "grid.hpp"
#include <algorithm>

Grid::Grid(int width, int height) : width(width), height(height) {
    grid.resize(width * height);
    for (int i = 0; i < grid.size() - 1; i++) {
        Cell newCell {CellType::none, i % width, i / width};
        grid[i] = newCell;
    }
}

void Grid::evolve(float dt) {
    for (int y = 0; y < height - 1; y++) {
        // Alternate processing rows left to right and right to left.
        const int startEnd[2] = {0, width - 1};
        int direction {y % 2 ? -1 : 1};
        for (int x = startEnd[y % 2]; x != startEnd[(y + 1) % 2] + direction; x += direction) {
            Cell &current {getCell(x, y)};
            if (current.type != CellType::none && current.type != CellType::wood) {
                moveCell(current, dt);
            }
        }
    }
}

void Grid::moveCell(Cell &cell, float dt) {
    // Swap the cell's internal position.
    sf::Vector2i desiredPos {cell.nextPosition(*this, dt)};
    int thisIndex {xyToI(cell.p.x, cell.p.y)};
    int desiredIndex {xyToI(desiredPos.x, desiredPos.y)};
    grid.at(desiredIndex).p = cell.p;
    cell.p = desiredPos;

    // Swap the cells.
    std::swap(grid.at(thisIndex), grid.at(desiredIndex));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////// Access functions /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

int Grid::xyToI(int x, int y) {
    return x + y * width;
}
