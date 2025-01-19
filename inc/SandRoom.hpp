#ifndef SAND_ROOM_HPP
#define SAND_ROOM_HPP

#include "Cell.hpp"
#include "Chunks.hpp"
#include "Elements/Names.hpp"
#include "Elements/ElementProperties.hpp"
#include "FreeList.h"
#include "Helpers.hpp"
#include <vector>
#include <tuple>

using roomID_t = int;

class MovementWorker;

struct Move {
    roomID_t srcRoomID;
    int src;
    int dst;

    Move() : srcRoomID(-1), src(-1), dst(-1) {}
    Move(roomID_t _srcRoomID, int _src, int _dst) : srcRoomID(_srcRoomID), src(_src), dst(_dst) {}
};

class SandRoom {
    friend MovementWorker;
    friend ActionWorker;
public:
    int x, y;
    int width, height;

    // The grid that actually contains the cells.
    Cells grid;
    // Contains the active chunks.
    Chunks chunks;

private:
    std::vector<Move> queuedMoves;
    std::vector<std::pair<size_t, Element>> queuedActions;

public:
    SandRoom(int _x, int _y, int _width, int _height, const ElementProperties * properties);

    void QueueMovement(roomID_t srcRoomID, int pFrom, int pTo);
    void QueueAction(size_t i, Element transform);

    // Access functions.
    CellState& GetCell(int index);
    CellState& GetCell(int x, int y);
    CellState& GetCell(sf::Vector2i p);

    // Setting functions.
    void SetCell(int index, Element id);
    void SetCell(int _x, int _y, Element id);

    // Querying the grid.
    bool IsEmpty(int _x, int _y);
    bool IsEmpty(sf::Vector2i p);
    bool InBounds(int _x, int _y) const;
    bool InBounds(sf::Vector2i p) const;

    // Helper functions.
    int ToIndex(int xw, int yw) const; // Converts world coordinates to a local index.
    int ToIndex(sf::Vector2i p) const;
    sf::Vector2i ToLocalCoords(int index) const;
    sf::Vector2i ToWorldCoords(int index) const;

};

#endif