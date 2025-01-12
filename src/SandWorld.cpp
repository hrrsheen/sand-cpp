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
    yMin(std::numeric_limits<int>::min()), yMax(std::numeric_limits<int>::max()) {
    InitProperties();
    SpawnRoom(0, 0);
}

SandWorld::SandWorld(int _xMin, int _xMax, int _yMin, int _yMax) : 
    xMin(_xMin), xMax(_xMax), yMin(_yMin), yMax(_yMax) {
    InitProperties();
    SpawnRoom(0, 0);
}

void SandWorld::InitProperties() {
    properties.Insert(std::move(std::make_unique<Sand>  (InitSand   ())));
    properties.Insert(std::move(std::make_unique<Stone> (InitStone  ())));
    properties.Insert(std::move(std::make_unique<Water> (InitWater  ())));
    properties.Insert(std::move(std::make_unique<Fire>  (InitFire   ())));
    properties.Insert(std::move(std::make_unique<Wood>  (InitWood   ())));
    properties.Insert(std::move(std::make_unique<Smoke> (InitSmoke  ())));
}

void SandWorld::SpawnRoom(int x, int y) {
    sf::Vector2i key {ToKey(x, y)};
    if (key.x > xMin && key.x < xMax && key.y > yMin && key.y < yMax) {
        SandRoom room {
            constants::roomWidth * x,
            constants::roomHeight * y,
            constants::roomWidth,
            constants::roomHeight,
            &properties
        };

        roomID_t id {rooms.Insert(room)};
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

SandRoom& SandWorld::GetRoom(sf::Vector2i key) {
    return rooms[roomsMap[key]];
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
    room.SetCell(x, y, id, properties.Get(id));
}

void SandWorld::SetArea(int x, int y, int w, int h, Element id) {
    SandRoom *room {nullptr};
    for (int yi = y; yi < y + h; ++yi) {
        for (int xi = x; xi < x + w; ++xi) {
            sf::Vector2i key {ToKey(xi, yi)};
            sf::Vector2i coords {xi, yi};
            if (!room || !room->InBounds(xi, yi)) 
                room = &rooms[roomsMap[key]];

            room->SetCell(xi, yi, id, properties.Get(id));
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
        return rooms[id].IsEmpty(p);
    }

    return false;
}

roomID_t SandWorld::EmptyRoom(sf::Vector2i p) {
    roomID_t id {ContainingRoomID(p)};
    if (VALID_ROOM(id) && rooms[id].IsEmpty(p)) {
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

size_t SandWorld::PropertiesSize() const {
    return properties.Size();
}

sf::Vector2i SandWorld::ToKey(int x, int y) {
    return sf::Vector2i {
        std::floor(x / constants::roomWidth), 
        std::floor(y / constants::roomHeight)
    };
}
