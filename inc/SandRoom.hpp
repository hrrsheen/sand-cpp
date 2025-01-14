#ifndef SAND_ROOM_HPP
#define SAND_ROOM_HPP

#include "Actions.hpp"
#include "Cell.hpp"
#include "Chunks.hpp"
#include "Elements/Names.hpp"
#include "Elements/ElementProperties.hpp"
#include "FreeList.h"
#include "Helpers.hpp"
#include <vector>
#include <tuple>

using roomID_t = int;

class SandRoom {
    using room_ptr = std::unique_ptr<SandRoom>;
public:
    const int x_m, y_m;
    const int width_m, height_m;

    // Contains the active chunks.
    Chunks chunks;

private:
    // The grid that actually contains the cells.
    std::vector<Cell> grid;

    std::vector<Move> queuedMoves;
    std::vector<std::pair<int, Element>> queuedActions;

public:
    SandRoom(int x, int y, int width, int height, const ElementProperties * properties);

    // Access functions.
    Cell& GetCell(int index);
    Cell& GetCell(int x, int y);
    Cell& GetCell(sf::Vector2i p);

    // Setting functions.
    void SetCell(int index, Element id);
    void SetCell(int x, int y, Element id);

    // Querying the grid.
    bool IsEmpty(int x, int y);
    bool IsEmpty(sf::Vector2i p);
    bool InBounds(int x, int y) const;
    bool InBounds(sf::Vector2i p) const;

    // Moving cells around.
    void QueueMove(roomID_t srcRoomID, int pFrom, int pTo);
    void ConsolidateMoves(FreeList<room_ptr> *rooms);

    // Transforming (applying actions) cells.
    void QueueAction(Action action);
    void ConsolidateActions();

    // Helper functions.
    int ToIndex(int xw, int yw) const; // Converts world coordinates to a local index.
    int ToIndex(sf::Vector2i p) const;
    sf::Vector2i ToLocalCoords(int index) const;
    sf::Vector2i ToWorldCoords(int index) const;

private:
    // Initialises each cell within the grid.
    // void InitCells(const ElementProperties * properties);
};

#endif