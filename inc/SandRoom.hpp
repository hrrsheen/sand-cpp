#ifndef SAND_ROOM_HPP
#define SAND_ROOM_HPP

#include "Cell.hpp"
#include "Chunks.hpp"
#include "Elements/Names.hpp"
#include "Elements/ElementProperties.hpp"
#include "Particles.hpp"
#include "Utility/Hashes.hpp"
#include <limits>
#include <vector>
#include <tuple>
#include <unordered_map>

using roomID_t = int;

class Rooms;

struct Move {
    roomID_t srcRoomID;
    int src;
    int dst;

    Move() : srcRoomID(-1), src(-1), dst(-1) {}
    Move(roomID_t _srcRoomID, int _src, int _dst) : srcRoomID(_srcRoomID), src(_src), dst(_dst) {}
};

class SandRoom {
public:
    int x, y;
    int width, height;

    // The grid that actually contains the cells.
    Cells grid;
    // Contains the active chunks.
    Chunks chunks;
    // Contains particles within the room.
    ParticleSystem particles;

private:
    std::vector<Move> queuedMoves;
    std::vector<std::pair<size_t, Element>> queuedActions;

public:
    SandRoom(int _x, int _y, int _width, int _height, const ElementProperties * properties);

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

    // Room mutation.
    void QueueMovement(roomID_t srcRoomID, int pFrom, int pTo);
    void QueueAction(size_t i, Element transform);

    void ConsolidateMovement(Rooms &rooms);
    void ConsolidateActions();

};

class Rooms {
public:
    using room_ptr = std::unique_ptr<SandRoom>;
    using iterator = std::vector<room_ptr>::iterator;
private:
    std::vector<room_ptr> rooms;

    size_t activeRooms = 0;
    size_t    maxRooms = std::numeric_limits<size_t>::max();

public:
    Rooms() = default;
    Rooms(size_t max) : maxRooms(max) {}

    // Inserts a new room and returns the index at which it was added. Returns -1 if no room is added.
    roomID_t NewRoom(int x, int y, int width, int height, const ElementProperties* properties);

    // Removes the room at the given index and returns the number of active rooms.
    void RemoveRoom(size_t index);

    size_t Range() const;
    bool Full() const;

    SandRoom* operator[](size_t n);

    // Returns the last active room in the container.
    SandRoom* Last() { return rooms[activeRooms - 1].get(); }
    // Iterator functions.
    iterator begin() { return rooms.begin(); }
    iterator end() { return rooms.end(); }
};

#endif