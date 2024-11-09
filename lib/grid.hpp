#ifndef GRID_HPP
#define GRID_HPP

#include "cell.hpp"
#include <vector>

class Grid {
public:
    Grid(int width, int height);

    // Advances the state of the grid by one iteration.
    void evolve(float dt);

    // Retrieves the cell from the grid at the given index.
    Cell& getCell(int index);

    // Retrieves the cell at the given coordinates from the grid.
    Cell& getCell(int x, int y);

    // Retrieves the cell at the given point from the grid.
    Cell& getCell(sf::Vector2i p);

    // Sets the cell at the given index to the given type.
    void setCell(int index, CellType type);

    // Sets the cell at the given coordinates to the given type.
    void setCell(int x, int y, CellType type);

    // Returns the number of elements in the grid.
    size_t size() const;

    // The width of the grid.
    const int width;

    // The height of the grid.
    const int height;

    friend Cell;

private:
    void moveCell(Cell &cell, float dt);

    int xyToI(int x, int y);

    // The grid that actually contains the cells.
    std::vector<Cell> grid;
};

#endif