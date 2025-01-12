#ifndef SAND_WORLD_HPP
#define SAND_WORLD_HPP

#include "Cell.hpp"
#include "Chunks.hpp"
#include "Elements/ElementProperties.hpp"
#include "Elements/PropertiesContainer.hpp"
#include "FreeList.h"
#include "Helpers.hpp"
#include "SandRoom.hpp"
#include <vector>
#include <unordered_map>

#define VALID_ROOM(id) ((id) > -1)

using roomID_t = int;

class SandWorld {
public:
    FreeList<SandRoom> rooms;
    // std::vector<SandRoom *> inactiveRooms;

private:
    std::unordered_map<sf::Vector2i, roomID_t> roomsMap;

    // The properties of the elements contained within the grid.
    PropertiesContainer properties;

    const int xMin, xMax, // The horizontal limits (number of rooms) of the world.
              yMin, yMax; // The vertical limits of the world.

public:
    SandWorld();
    SandWorld(int _xMin, int _xMax, int _yMin, int _yMax);

    // Access functions.
    Cell &GetCell(int x, int y);
    SandRoom& GetRoom(sf::Vector2i key);
    SandRoom& GetContainingRoom(sf::Vector2i p);
    SandRoom& GetContainingRoom(int x, int y);

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

    // Helper functions.
    size_t Size() const; // Returns the number of active rooms.
    size_t PropertiesSize() const;

private:
    // Populated the properties container with the properties defined in the given config file.
    void InitProperties();

    void SpawnRoom(int x, int y);
    void RemoveRoom(int x, int y);

    // Returns the key to the room that contains the point (x, y).
    sf::Vector2i ToKey(int x, int y);

};

template <> 
struct std::hash<sf::Vector2i> {
    std::size_t operator()(sf::Vector2i const &p) const noexcept {
        std::size_t h {std::hash<int>{}(p.x)};
        HashCombine(h, p.y);
        return h;
    }
};

#endif