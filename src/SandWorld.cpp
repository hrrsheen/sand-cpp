#include "Cell.hpp"
#include "Constants.hpp"
#include "Elements.hpp"
#include "SandWorld.hpp"
#include <algorithm>
#include <limits>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////////////////
//  Initialisation.
//////////////////////////////////////////////////////////////////////////////////////////

SandWorld::SandWorld() : 
    xMin(std::numeric_limits<int>::min()), xMax(std::numeric_limits<int>::max()),
    yMin(std::numeric_limits<int>::min()), yMax(std::numeric_limits<int>::max()),
    properties() {
    SpawnRoom(0, 0);
}

SandWorld::SandWorld(int _xMin, int _xMax, int _yMin, int _yMax, size_t maxRooms) : 
    xMin(_xMin), xMax(_xMax), yMin(_yMin), yMax(_yMax), rooms(maxRooms),
    properties() {
    if (maxRooms < 4) {
        throw std::invalid_argument("maxRooms must be >= 4.");
    }
    SpawnRoom(0, 0);
}

roomID_t SandWorld::SpawnRoom(int x, int y) {
    sf::Vector2i key {ToKey(x, y)};
    if (roomsMap.count(key)) return roomsMap[key];
    if (key.x >= xMin && key.x < xMax && key.y >= yMin && key.y < yMax) {
        if (rooms.Full()) { RemoveFurthestRoom(x, y); } // TODO: ensure that sand/particles can't move into unspawned rooms when they're full.
        roomID_t id = rooms.NewRoom(
            key.x * constants::roomWidth,
            key.y * constants::roomHeight,
            constants::roomWidth,
            constants::roomHeight,
            &properties);
        roomsMap[key] = id;
        return id;
    }
    throw std::runtime_error("Failed to spawn SandRoom.");
}

void SandWorld::RemoveFurthestRoom(int x, int y) {
    // The Signed Distance Field function for a rectangle, centres on (0, 0) with width 2Rx and height 2Ry.
    auto rectSDF = [](int Px, int Py, int Rx, int Ry) {
        int Dx = std::abs(Px) - Rx, Dy = std::abs(Py) - Ry;
        int outLen = std::sqrt(std::max(Dx, 0) * std::max(Dx, 0) + std::max(Dy, 0) * std::max(Dy, 0));
        int inLen = std::min(std::max(Dx, Dy), 0);
        return outLen + inLen;
    };

    int furthestIndex   = 0;
    int furthestDist    = 0;
    for (int i = 0; i < rooms.Range(); i++) {
        SandRoom *current = rooms[i];
        // Transform for the point to be relative to a centred rectangle.
        sf::Vector2i T {current->x + current->width / 2, current->y + current->height / 2};
        // Calculate the distance to the room border.
        int dist = std::abs(rectSDF(x - T.x, y - T.y, current->width / 2, current->height / 2));
        if (dist > furthestDist) {
            furthestIndex   = i;
            furthestDist    = dist;
        }
    }

    // Update the mapping with the index of the room that's been swapped with the furthest room.
    sf::Vector2i keyReplacement {ToKey(rooms.Last()->x,         rooms.Last()->y)};
    sf::Vector2i keyRemoved     {ToKey(rooms[furthestIndex]->x, rooms[furthestIndex]->y)};
    std::swap(roomsMap[keyReplacement], roomsMap[keyRemoved]);
    roomsMap.erase(keyRemoved);

    rooms.RemoveRoom(furthestIndex);
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Access Functions.
//////////////////////////////////////////////////////////////////////////////////////////

CellState& SandWorld::GetCell(int x, int y) {
    return GetContainingRoom(x, y).GetCell(x, y);
}

size_t SandWorld::CellIndex(sf::Vector2i p) {
    return GetContainingRoom(p).ToIndex(p);
}

SandRoom& SandWorld::GetRoom(roomID_t id) {
    return *rooms[id];
}

SandRoom& SandWorld::GetRoom(sf::Vector2i key) {
    return GetRoom(roomsMap.at(key));
}

SandRoom& SandWorld::GetContainingRoom(sf::Vector2i p) {
    return GetRoom(ToKey(p.x, p.y));
}

SandRoom& SandWorld::GetContainingRoom(int x, int y) {
    return GetRoom(ToKey(x, y));
}

const ConstProperties& SandWorld::GetProperties(sf::Vector2i p) {
    SandRoom &room {GetContainingRoom(p.x, p.y)};
    return room.grid.GetProperties(room.ToIndex(p));
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Setting functions.
//////////////////////////////////////////////////////////////////////////////////////////

void SandWorld::SetCell(int x, int y, Element id) {
    roomID_t roomID {ContainingRoomID(sf::Vector2i(x, y))};
    if (VALID_ROOM(roomID)) {
        SandRoom &room {GetRoom(roomID)};
        room.SetCell(x, y, id);
    }
}

void SandWorld::SetArea(int x, int y, int w, int h, Element id) {
    SandRoom *room {nullptr};
    roomID_t roomID {-1}, prevRoomID {-1};
    for (int yi = y; yi <= y + h; ++yi) {
        for (int xi = x; xi <= x + w; ++xi) {
            sf::Vector2i coords {xi, yi};
            roomID_t roomID {ContainingRoomID(coords)};
            if (VALID_ROOM(roomID)) {
                if (roomID != prevRoomID) {
                    room = &GetRoom(roomID);
                    prevRoomID = roomID;
                }
                room->SetCell(xi, yi, id);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Querying the grid.
//////////////////////////////////////////////////////////////////////////////////////////

bool SandWorld::IsEmpty(int x, int y) {
    sf::Vector2i p {x, y};
    return IsEmpty(p);
}

bool SandWorld::IsEmpty(sf::Vector2i p) {
    roomID_t id {ContainingRoomID(p)};
    if (VALID_ROOM(id)) {
        return GetRoom(id).IsEmpty(p);
    }

    return false;
}

roomID_t SandWorld::EmptyRoom(sf::Vector2i p) {
    roomID_t id {ContainingRoomID(p)};
    if (VALID_ROOM(id) && GetRoom(id).IsEmpty(p)) {
        return id;
    }

    return -1;
}

roomID_t SandWorld::ContainingRoomID(sf::Vector2i p) {
    sf::Vector2i key {ToKey(p.x, p.y)};
    if (roomsMap.count(key)) {
        return roomsMap[key];
    }
    
    return -1;
}

bool SandWorld::InBounds(sf::Vector2i p) {
    return p.x >= xMin * constants::roomWidth && p.x < xMax * constants::roomWidth
        && p.y >= yMin * constants::roomHeight && p.y < yMax * constants::roomHeight;
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Helper functions.
//////////////////////////////////////////////////////////////////////////////////////////

size_t SandWorld::Size() const {
    return rooms.Range();
}

sf::Vector2i SandWorld::ToKey(int x, int y) {
    return sf::Vector2i {
        static_cast<int>(std::floor(static_cast<float>(x) / constants::roomWidth)), 
        static_cast<int>(std::floor(static_cast<float>(y) / constants::roomHeight))
    };
}
