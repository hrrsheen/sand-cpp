#include "Cell.hpp"
#include "Constants.hpp"
#include "Elements.hpp"
#include "Helpers.hpp"
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
    if (!InitProperties()) {
        throw std::runtime_error("Failed to initialise ElementProperties.");
    }
    SpawnRoom(0, 0);
}

SandWorld::SandWorld(int _xMin, int _xMax, int _yMin, int _yMax) : 
    xMin(_xMin), xMax(_xMax), yMin(_yMin), yMax(_yMax),
    properties() {
    if (!InitProperties()) {
        throw std::runtime_error("Failed to initialise ElementProperties.");
    }
    SpawnRoom(0, 0);
}

bool SandWorld::InitProperties() {
    bool success = false;
    success |= InitSand (properties);
    success |= InitStone(properties);
    success |= InitWood (properties);
    success |= InitWater(properties);
    success |= InitFire (properties);
    success |= InitSmoke(properties);

    return success;
}

void SandWorld::SpawnRoom(int x, int y) {
    sf::Vector2i key {ToKey(x, y)};
    ElementProperties* propPtr {&properties};
    if (key.x >= xMin && key.x < xMax && key.y >= yMin && key.y < yMax) {        
        room_ptr roomPtr (std::make_unique<SandRoom>(
            constants::roomWidth * x,
            constants::roomHeight * y,
            constants::roomWidth,
            constants::roomHeight,
            propPtr
        ));
        roomID_t id {rooms.Insert(std::move(roomPtr))};
        roomsMap[key] = id;
    }
}

void SandWorld::RemoveRoom(int x, int y) {
    sf::Vector2i key {x, y};
    roomID_t id {roomsMap.at(key)};
    rooms.Erase(id);
    roomsMap.erase(key);
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Access Functions.
//////////////////////////////////////////////////////////////////////////////////////////

Cell& SandWorld::GetCell(int x, int y) {
    return GetContainingRoom(x, y).GetCell(x, y);
}

SandRoom& SandWorld::GetRoom(roomID_t id) {
    return *rooms[id].get();
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

//////////////////////////////////////////////////////////////////////////////////////////
//  Setting functions.
//////////////////////////////////////////////////////////////////////////////////////////

void SandWorld::SetCell(int x, int y, Element id) {
    SandRoom &room {GetContainingRoom(x, y)};
    room.SetCell(x, y, id);
}

void SandWorld::SetArea(int x, int y, int w, int h, Element id) {
    SandRoom *room {nullptr};
    roomID_t roomID {-1}, prevRoomID {-1};
    for (int yi = y; yi < y + h; ++yi) {
        for (int xi = x; xi < x + w; ++xi) {
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
