#include "Cell.hpp"
#include "Constants.hpp"
#include "Elements.hpp"
#include "SandRoom.hpp"
#include "Utility/Random.hpp"
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


//////////////////////////////////////////////////////////////////////////////////////////
//  Rooms container functions.
//////////////////////////////////////////////////////////////////////////////////////////

roomID_t Rooms::NewRoom(int x, int y, int width, int height, const ElementProperties* properties) {
    if (activeRooms >= maxRooms) return -1;

    room_ptr room {std::make_unique<SandRoom>(x, y, width, height, properties)};
    if (activeRooms == rooms.size()) {
        rooms.push_back(std::move(room));
    } else {
        rooms[activeRooms] = std::move(room);
    }

    activeRooms++;
    return activeRooms - 1;
}

void Rooms::RemoveRoom(size_t index) {
    activeRooms--;
    std::swap(rooms[index], rooms[activeRooms]);

    rooms.erase(rooms.begin() + activeRooms);
}

size_t Rooms::Range() const {
    return activeRooms;
}

bool Rooms::Full() const {
    return activeRooms >= maxRooms;
}

SandRoom* Rooms::operator[](size_t n) {
    return rooms[n].get();
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Rooms mutationfunctions.
//////////////////////////////////////////////////////////////////////////////////////////

void SandRoom::QueueMovement(roomID_t srcRoomID, int pFrom, int pTo) {
    queuedMoves.emplace_back(srcRoomID, pFrom, pTo);
}

void SandRoom::QueueAction(size_t i, Element transform) {
    queuedActions.emplace_back(i, transform);
}

void SandRoom::ConsolidateMovement(Rooms &rooms) {
    if (queuedMoves.size() == 0) return;

    // Remove moves that have had their destination filled between frames.
    // TODO. Will probably be needed for threading?

    // Sort moves by source rooms then by destination.
    std::sort(queuedMoves.begin(), queuedMoves.end(), 
        [](const Move &a, const Move &b) { return a.dst < b.dst; }
    );
    
    // As the vector is sorted, this end object is guaranteed to be different to any other element.
    queuedMoves.emplace_back();

    int iStart {0};

    for (int i = 0; i < queuedMoves.size() - 1; ++i) {
        Move move       {queuedMoves.at(i)};
        Move nextMove   {queuedMoves.at(i + 1)};

        if (move.dst != nextMove.dst) {
            // Perform the randomly-selected move from the competing moves group.
            int iRand {iStart + QuickRandInt(i - iStart)};
            
            roomID_t id {queuedMoves[iRand].srcRoomID};
            int src     {queuedMoves[iRand].src};
            int dst     {queuedMoves[iRand].dst};
            
            SandRoom *srcRoom {rooms[id]};
            std::swap(srcRoom->grid.state[src], grid.state[dst]);
            std::swap(srcRoom->grid.colour[src], grid.colour[dst]);

            sf::Vector2i srcCoords {srcRoom->ToWorldCoords(src)};
            sf::Vector2i dstCoords {         ToWorldCoords(dst)};
            srcRoom->chunks.KeepContainingAlive(srcCoords.x, srcCoords.y);
                     chunks.KeepContainingAlive(dstCoords.x, dstCoords.y);

            iStart = i + 1;
        }
    }

    queuedMoves.clear();
}

void SandRoom::ConsolidateActions() {
    if (queuedActions.size() == 0) return;

    // Sort the queued actions by destination.
    std::sort(queuedActions.begin(), queuedActions.end(), 
        [](const std::pair<size_t, Element> &a, const std::pair<size_t, Element> &b) { 
            return a.first < b.first;
        }
    );

    // Used to catch the final action. 
    queuedActions.emplace_back(-1, Element::null);

    int iStart {0};

    for (int i = 0; i < queuedActions.size() - 1; ++i) {
        std::pair<size_t, Element> move       {queuedActions.at(i)};
        std::pair<size_t, Element> nextMove   {queuedActions.at(i + 1)};

        if (move.first != nextMove.first) {
            // Perform the randomly-selected action from the competing actions group.
            int iRand {iStart + QuickRandInt(i - iStart)};
            
            size_t iCell {queuedActions[iRand].first};
            Element tfID {queuedActions[iRand].second};
            grid.Assign(iCell, tfID);

            sf::Vector2i coords {ToWorldCoords(iCell)};
            chunks.KeepContainingAlive(coords.x, coords.y);
        
            iStart = i + 1;
        }
    }

    queuedActions.clear();
}