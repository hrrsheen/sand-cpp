#include "cell.hpp"
#include "elements/solid_movable.hpp"
#include "grid.hpp"
#include <algorithm>

Grid::Grid(int width, int height) : width(width), height(height) {
    grid.resize(width * height);
}

void Grid::initProperties() {
    properties.insert(std::move(std::make_unique<Sand>()));
}

void Grid::initCells() {
    for (int i = 0; i < grid.size(); i++) {
        Cell newCell {i % width, i / width, &properties};
        // newCell.redraw = true;
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

void Grid::setCell(int index, Element elementId) {
    grid.at(index).assign(elementId);
}

void Grid::setCell(int x, int y, Element elementId) {
    grid.at(x + y * width).assign(elementId);
}

void Grid::setArea(int x, int y, int w, int h, Element elementId) {
    if (x < 0) {
        w = x + w;
        x = 0;
    } else if (x + w > width) {
        w = width - x;
    }

    if (y < 0) {
        h = y + h;
        y = 0;
    } else if (y + h > height) {
        h = height - y;
    }

    for (int yi = y; yi < y + h; yi++) {
        for (int xi = x; xi < x + w; xi++) {
            // TODO: don't overwrite non-air cells.
            setCell(xi, yi, elementId);
        }
    }
}

ElementProperties& Grid::getProperties(Cell &cell) {
    return properties.get(cell.elementId);
}

size_t Grid::size() const {
    return grid.size();
}

int Grid::coordsToIndex(int x, int y) const {
    return x + y * width;
}
