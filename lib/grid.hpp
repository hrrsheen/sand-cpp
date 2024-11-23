#ifndef GRID_HPP
#define GRID_HPP

#include "cell.hpp"
#include <vector>

class Grid {
public:
    Grid(int width, int height);

    // Populated the properties container with the properties defined in the given config file.
    void initProperties(std::vector<std::string> propertiesStr); // TODO: Make this use JSON.

    // Initialises each cell within the grid.
    void initCells();

    // Retrieves the cell from the grid at the given index.
    Cell& getCell(int index);

    // Retrieves the cell at the given coordinates from the grid.
    Cell& getCell(int x, int y);

    // Retrieves the cell at the given point from the grid.
    Cell& getCell(sf::Vector2i p);

    // Sets the cell at the given index to the given type.
    void setCell(int index, int elementId);

    // Sets the cell at the given coordinates to the given type.
    void setCell(int x, int y, int elementId);

    // Sets the rectangle at (x, y) with the given width and height to the given element ID.
    void setArea(int x, int y, int width, int height, int elementId);

    // Retrieves the properties of a given cell.
    ElementProperties& getProperties(Cell &cell);

    // Returns the number of elements in the grid.
    size_t size() const;

    // Returns the index of the given (x, y) coordinate within the grid.
    int coordsToIndex(int x, int y);

    // The width of the grid.
    const int width;

    // The height of the grid.
    const int height;

    friend Cell;

private:
    void moveCell(Cell &cell, float dt);

    // The grid that actually contains the cells.
    std::vector<Cell> grid;

    // The properties of the elements contained within the grid.
    PropertiesContainer properties; // TODO: Surely there's a better way to access by id than id i = index i.
};

#endif