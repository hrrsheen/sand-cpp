#ifndef SAND_WORLD_HPP
#define SAND_WORLD_HPP

#include "Cell.hpp"
#include "Chunks.hpp"
#include "Elements/ElementProperties.hpp"
#include "Elements/PropertiesContainer.hpp"
#include <vector>

class SandWorld {
public:
    // Contains the active chunks.
    Chunks chunks;

    // The width of the grid.
    const int width;

    // The height of the grid.
    const int height;

    friend Cell;

private:
    // The grid that actually contains the cells.
    std::vector<Cell> grid;

    std::vector<std::pair<int, int>> queuedMoves;

    // The properties of the elements contained within the grid.
    PropertiesContainer properties; // TODO: Surely there's a better way to access by id than id i = index i.

public:
    SandWorld(int width, int height);
    // Initialisation functions.
    // Populated the properties container with the properties defined in the given config file.
    void InitProperties();
    // Initialises each cell within the grid.
    void InitCells();

    // Access functions.
    Cell& GetCell(int index);
    Cell& GetCell(int x, int y);
    Cell& GetCell(sf::Vector2i p);
    // Retrieves the properties of a given cell.
    ElementProperties& GetProperties(Cell &cell);

    // Setting functions.
    void SetCell(int index, Element elementId);
    void SetCell(int x, int y, Element elementId);
    // Sets the rectangle at (x, y) with the given width and height to the given element ID.
    void SetArea(int x, int y, int width, int height, Element elementId);

    // Moving cells around.
    void MoveCell(int pFrom, int pTo);
    void ConsolidateActions();
    void Swap(int i1, int i2);

    // Querying the grid.
    bool IsEmpty(int x, int y);
    bool InBounds(int x, int y) const;

    // Helper functions.
    size_t Size() const;
    int ToIndex(int x, int y) const;
    sf::Vector2i ToCoords(int index) const;
};

#endif