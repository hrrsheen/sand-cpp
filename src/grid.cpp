#include "grid.hpp"
#include <algorithm>

Grid::Grid(int width, int height) : width(width), height(height) {
    grid.resize(width * height);
}

void Grid::initProperties(std::vector<std::string> propertiesStr) {
    for (std::string &st : propertiesStr) {
        ElementProperties elProperties{};
        elProperties.loadFromFile(st);
        properties.insert(elProperties);
    }
}

void Grid::initCells() {
    for (int i = 0; i < grid.size() - 1; i++) {
        Cell newCell;
        newCell.p.y = i / width;
        newCell.p.x = i % width;
        newCell.assign(0, properties.get(0)); // TODO: Need to make this not use magic numbers.
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

void Grid::setCell(int index, int elementId) {
    grid.at(index).assign(elementId, properties.get(elementId));
}

void Grid::setCell(int x, int y, int elementId) {
    grid.at(x + y * width).assign(elementId, properties.get(elementId));
}

void Grid::setArea(int x, int y, int w, int h, int elementId) {
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
    if (cell.elementId == -1) {
        ElementProperties properties{}; // Return the null properties.
        return properties;
    }
    return properties.get(cell.elementId);
}

size_t Grid::size() const {
    return grid.size();
}

int Grid::coordsToIndex(int x, int y) {
    return x + y * width;
}
