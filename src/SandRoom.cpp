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
    chunks(constants::numXChunks, constants::numYChunks, constants::chunkWidth, constants::chunkHeight) {
    Cell defaultCell(properties);
    grid.resize(width * height, defaultCell);
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Access Functions.
//////////////////////////////////////////////////////////////////////////////////////////

Cell& SandRoom::GetCell(int index) {
    return grid.at(index);
}

Cell& SandRoom::GetCell(int _x, int _y) {
    return GetCell(ToIndex(_x, _y));
}

Cell& SandRoom::GetCell(sf::Vector2i p) {
    return GetCell(ToIndex(p.x, p.y));
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Setting functions.
//////////////////////////////////////////////////////////////////////////////////////////

void SandRoom::SetCell(int index, Element id) {
    sf::Vector2i coords {ToLocalCoords(index)};
    GetCell(index).Assign(id, coords.x, coords.y);
    chunks.KeepContainingAlive(coords.x, coords.y);
}

void SandRoom::SetCell(int _x, int _y, Element id) {
    GetCell(_x, _y).Assign(id, _x, _y);
    chunks.KeepContainingAlive(_x, _y);
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Querying the grid.
//////////////////////////////////////////////////////////////////////////////////////////

bool SandRoom::IsEmpty(int _x, int _y) {
    if (InBounds(_x, _y)) return GetCell(_x, _y).id == Element::air;

    return false;
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
//  Moving cells.
//////////////////////////////////////////////////////////////////////////////////////////

void SandRoom::QueueMove(roomID_t srcRoomID, int pFrom, int pTo) {
    queuedMoves.emplace_back(srcRoomID, pFrom, pTo);
}

void SandRoom::ConsolidateMoves(FreeList<room_ptr> *rooms) {
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
            int src {queuedMoves[iRand].src};
            int dst {queuedMoves[iRand].dst};
            
            SandRoom *room {(*rooms)[id].get()};
            Cell tmp {room->GetCell(src)};
            Cell &srcCell {GetCell(src)};
            Cell &dstCell {GetCell(dst)};
            dstCell.redraw = true;
            tmp.redraw = true;
            srcCell = dstCell;
            dstCell = tmp;

            sf::Vector2i srcCoords {ToWorldCoords(src)};
            sf::Vector2i dstCoords {ToWorldCoords(dst)};
            chunks.KeepContainingAlive(srcCoords.x, srcCoords.y);
            chunks.KeepContainingAlive(dstCoords.x, dstCoords.y);

            iStart = i + 1;
        }
    }

    queuedMoves.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Applying actions.
//////////////////////////////////////////////////////////////////////////////////////////

void SandRoom::QueueAction(Action action) {
    if (ToIndex(action.p) < 0) {

    }
    queuedActions.emplace_back(ToIndex(action.p), action.transform);
}

void SandRoom::ConsolidateActions() {
    if (queuedActions.size() == 0) return;

    // Sort the queued actions by destination.
    std::sort(queuedActions.begin(), queuedActions.end(), 
        [](const std::pair<int, Element> &a, const std::pair<int, Element> &b) { 
            return a.first < b.first;
        }
    );

    // Used to catch the final action. 
    queuedActions.emplace_back(-1, Element::null);

    int iStart {0};

    for (int i = 0; i < queuedActions.size() - 1; ++i) {
        std::pair<int, Element> move       {queuedActions.at(i)};
        std::pair<int, Element> nextMove   {queuedActions.at(i + 1)};

        if (move.first != nextMove.first) {
            // Perform the randomly-selected action from the competing actions group.
            int iRand {iStart + QuickRandInt(i - iStart)};
            
            int iCell    {queuedActions[iRand].first};
            Element tfID {queuedActions[iRand].second};
            SetCell(iCell, tfID);

            sf::Vector2i coords {ToWorldCoords(iCell)};
            chunks.KeepContainingAlive(coords.x, coords.y);
        
            iStart = i + 1;
        }
    }

    queuedActions.clear();
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