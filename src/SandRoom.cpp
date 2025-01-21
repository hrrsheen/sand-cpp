#include "Cell.hpp"
#include "Constants.hpp"
#include "Elements.hpp"
#include "Helpers.hpp"
#include "SandRoom.hpp"
#include <algorithm>

//////////////////////////////////////////////////////////////////////////////////////////
//  Initialisation Functions.
//////////////////////////////////////////////////////////////////////////////////////////

SandRoom::SandRoom(int _x, int _y, int _width, int _height, const ElementProperties * properties) : 
    x(_x), y(_y), width(_width), height(_height), 
    grid(_width, _height, properties),
    chunks(constants::numXChunks, constants::numYChunks, constants::chunkWidth, constants::chunkHeight, x, y) {}

//////////////////////////////////////////////////////////////////////////////////////////
//  Access Functions.
//////////////////////////////////////////////////////////////////////////////////////////

void SandRoom::QueueMovement(roomID_t srcRoomID, int pFrom, int pTo) {
    queuedMoves.emplace_back(srcRoomID, pFrom, pTo);
}

void SandRoom::QueueAction(size_t i, Element transform) {
    queuedActions.emplace_back(i, transform);
}

CellState& SandRoom::GetCell(int index) {
    return grid.state.at(index);
}

CellState& SandRoom::GetCell(int _x, int _y) {
    return GetCell(ToIndex(_x, _y));
}

CellState& SandRoom::GetCell(sf::Vector2i p) {
    return GetCell(ToIndex(p.x, p.y));
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Setting functions.
//////////////////////////////////////////////////////////////////////////////////////////

void SandRoom::SetCell(int index, Element id) {
    sf::Vector2i coords {ToLocalCoords(index)};
    grid.Assign(index, id, coords.x, coords.y);
    chunks.KeepContainingAlive(coords.x, coords.y);
}

void SandRoom::SetCell(int _x, int _y, Element id) {
    grid.Assign(ToIndex(_x, _y), id, _x, _y);
    chunks.KeepContainingAlive(_x, _y);
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Querying the grid.
//////////////////////////////////////////////////////////////////////////////////////////

bool SandRoom::IsEmpty(int _x, int _y) {
    if (InBounds(_x, _y)) {
        return GetCell(ToIndex(_x, _y)).id == Element::air;
    } else {
        return false;   
    }
}

bool SandRoom::IsEmpty(sf::Vector2i p) {
    return IsEmpty(p.x, p.y);
}

bool SandRoom::InBounds(int _x, int _y) const {
    return _x >= x && _x < x + width 
        && _y >= y && _y < y + height;
}

bool SandRoom::InBounds(sf::Vector2i p) const {
    return InBounds(p.x, p.y);
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Helper functions.
//////////////////////////////////////////////////////////////////////////////////////////

int SandRoom::ToIndex(int xw, int yw) const {
    return (xw - x) + (yw - y) * width;
}

int SandRoom::ToIndex(sf::Vector2i p) const {
    return ToIndex(p.x, p.y);
}

sf::Vector2i SandRoom::ToLocalCoords(int index) const {
    int y {index / width};
    int x {index % width};

    return sf::Vector2i(x, y);
}

sf::Vector2i SandRoom::ToWorldCoords(int index) const {
    sf::Vector2i coords {ToLocalCoords(index)};
    coords.x += x;
    coords.y += y;

    return coords;
}