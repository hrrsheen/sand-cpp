#ifndef SAND_WORLD_HPP
#define SAND_WORLD_HPP

#include "Cell.hpp"
#include "Chunks.hpp"
#include "Constants.hpp"
#include "Elements/ElementProperties.hpp"
#include "FreeList.h"
#include "SandRoom.hpp"
#include "Utility/Hashes.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>

#define VALID_ROOM(id) ((id) > -1)

using roomID_t = int;

struct Vector2iHash {
    std::size_t operator()(sf::Vector2i const &p) const {
        std::size_t h {std::hash<int>{}(p.x)};
        HashCombine(h, p.y);
        return h;
    }
};

class SandWorld {
    using room_ptr = std::unique_ptr<SandRoom>;
public:
    FreeList<room_ptr> rooms;
    
    // The properties of the elements being simulated in the world.
    ElementProperties properties;

private:
    std::unordered_map<sf::Vector2i, roomID_t, Vector2iHash> roomsMap;

    const int xMin, xMax, // The horizontal limits (number of rooms) of the world.
              yMin, yMax; // The vertical limits of the world.

public:
    SandWorld();
    SandWorld(int _xMin, int _xMax, int _yMin, int _yMax);

    roomID_t SpawnRoom(int x, int y);
    roomID_t RemoveRoom(int x, int y);

    // Access functions.
    CellState &GetCell(int x, int y);
    size_t CellIndex(sf::Vector2i p);
    SandRoom& GetRoom(roomID_t id);
    SandRoom& GetRoom(sf::Vector2i key);
    SandRoom& GetContainingRoom(sf::Vector2i p);
    SandRoom& GetContainingRoom(int x, int y);

    const ConstProperties& GetProperties(sf::Vector2i p);

    // Setting functions.
    void SetCell(int x, int y, Element id);
    // Sets the rectangle at (x, y) with the given width and height to the given element ID.
    void SetArea(int x, int y, int width, int height, Element id);

    // Querying the grid.
    bool IsEmpty(int x, int y);
    bool IsEmpty(sf::Vector2i p);
    // If the cell at position p is empty, return the ID of its containing room. Return -1 otherwise.
    roomID_t EmptyRoom(sf::Vector2i p);
    // If a room that contains point p exists, return its ID. Otherwise return -1.
    roomID_t ContainingRoomID(sf::Vector2i p); 
    // Returns true if p is within the boundaries of all possible rooms.
    bool InBounds(sf::Vector2i p);

    // Helper functions.
    size_t Size() const; // Returns the number of active rooms.

private:
    // Populated the properties container. Returns true if successful, false otherwise.
    bool InitProperties();

    // Returns the key to the room that contains the point (x, y).
    sf::Vector2i ToKey(int x, int y);

};

#endif