#include "Cell.hpp"
#include "Constants.hpp"
#include "Elements.hpp"
#include "Helpers.hpp"
#include "SandRoom.hpp"
#include <algorithm>

//////////////////////////////////////////////////////////////////////////////////////////
//  Initialisation Functions.
//////////////////////////////////////////////////////////////////////////////////////////

SandRoom::SandRoom(int x, int y, int width, int height, PropertiesContainer *properties) : 
    x_m(x), y_m(y), width_m(width), height_m(height), 
    chunks(constants::numXChunks, constants::numYChunks, constants::chunkWidth, constants::chunkHeight) {
    grid.resize(width_m * height_m);
    InitCells(properties);
}

void SandRoom::InitCells(PropertiesContainer *properties) {
    for (int i = 0; i < grid.size(); ++i) {
        Cell newCell;
        newCell.Assign(Element::air, properties->Get(Element::air), i / width_m);
        grid[i] = newCell;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Access Functions.
//////////////////////////////////////////////////////////////////////////////////////////

Cell& SandRoom::GetCell(int index) {
    return grid.at(index);
}

Cell& SandRoom::GetCell(int x, int y) {
    return GetCell(ToIndex(x, y));
}

Cell& SandRoom::GetCell(sf::Vector2i p) {
    return GetCell(ToIndex(p.x, p.y));
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Setting functions.
//////////////////////////////////////////////////////////////////////////////////////////

void SandRoom::SetCell(int index, Element id, ElementProperties &properties) {
    sf::Vector2i coords {ToLocalCoords(index)};
    GetCell(index).Assign(id, properties, coords.x, coords.y);
    chunks.KeepContainingAlive(coords.x, coords.y);
}

void SandRoom::SetCell(int x, int y, Element id, ElementProperties &properties) {
    GetCell(x, y).Assign(id, properties, x, y);
    chunks.KeepContainingAlive(x, y);
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Querying the grid.
//////////////////////////////////////////////////////////////////////////////////////////

bool SandRoom::IsEmpty(int x, int y) {
    if (InBounds(x, y)) return GetCell(x, y).id == Element::air;

    return false;
}

bool SandRoom::IsEmpty(sf::Vector2i p) {
    return IsEmpty(p.x, p.y);
}

bool SandRoom::InBounds(int x, int y) const {
    return x >= x_m && x < x_m + width_m 
        && y >= y_m && y < y_m + height_m;
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

void SandRoom::ConsolidateMoves(FreeList<SandRoom> *rooms) {
    if (queuedMoves.size() == 0) return;

    // Remove moves that have had their destination filled between frames.
    // TODO. Will probably be needed for threading?

    // Sort moves by source rooms then by destination.
    std::sort(queuedMoves.begin(), queuedMoves.end(), 
        [](const Move &a, const Move &b) { a.dst < b.dst; }
    );
    
    // As the vector is sorted, this end object is guaranteed to be different to any other element.
    queuedMoves.emplace_back();

    int iStart {0};

    for (int i = 0; i < queuedMoves.size() - 1; ++i) {
        Move &move       {queuedMoves.at(i)};
        Move &nextMove   {queuedMoves.at(i + 1)};

        if (move.dst != nextMove.dst) {
            // Perform the randomly-selected move from the competing moves group.
            int iRand {iStart + QuickRandInt(i - iStart)};
            
            roomID_t id;
            int src {queuedMoves[iRand].src};
            int dst {queuedMoves[iRand].dst};
            
            Cell tmp {(*rooms)[id].GetCell(src)};
            (*rooms)[id].GetCell(src) = GetCell(dst); // Does this even work? lol.
            GetCell(dst) = tmp;

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
    queuedActions.emplace_back(ToIndex(action.p), action.transform);
}

void SandRoom::ConsolidateActions(PropertiesContainer &properties) {
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
        std::pair<int, Element> &move       {queuedActions.at(i)};
        std::pair<int, Element> &nextMove   {queuedActions.at(i + 1)};

        if (move.first != nextMove.first) {
            // Perform the randomly-selected action from the competing actions group.
            int iRand {iStart + QuickRandInt(i - iStart)};
            
            int iCell    {queuedActions[iRand].first};
            Element tfID {queuedActions[iRand].second};
            SetCell(iCell, tfID, properties.Get(tfID));

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
    return (xw - x_m) + (yw - y_m) * width_m;
}

int SandRoom::ToIndex(sf::Vector2i p) const {
    return ToIndex(p.x, p.y);
}

sf::Vector2i SandRoom::ToLocalCoords(int index) const {
    int y {index / width_m};
    int x {index % width_m};

    return sf::Vector2i(x, y);
}

sf::Vector2i SandRoom::ToWorldCoords(int index) const {
    sf::Vector2i coords {ToLocalCoords(index)};
    coords.x += x_m;
    coords.y += y_m;

    return coords;
}