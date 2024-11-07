#ifndef GRID_HPP
#define GRID_HPP

#include <cstdint>
#include <vector>

enum CellType : uint8_t {
    none,
    sand,
    water,
    wood
};

struct Cell {
    CellType type;
    int x, y; // The (x, y) coordinate of this cell within its grid.
};

class Grid {
public:
    Grid(int width, int height);

    // Retrieves the cell from the grid at the given index.
    Cell& getCell(int index);

    // Retrieves the cell at the given coordinates from the grid.
    Cell& getCell(int x, int y);

    // Sets the cell at the given index to the given type.
    void setCell(int index, CellType type);

    // Sets the cell at the given coordinates to the given type.
    void setCell(int x, int y, CellType type);

    // Returns the number of elements in the grid.
    size_t size();

private:
    // The width of the grid.
    const int width;

    // The height of the grid.
    const int height;

    // The grid that actually contains the cells.
    std::vector<Cell> grid;
};

#endif