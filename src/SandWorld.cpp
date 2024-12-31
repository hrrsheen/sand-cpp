#include "Cell.hpp"
#include "Elements.hpp"
#include "Helpers.hpp"
#include "SandWorld.hpp"
#include <algorithm>

//////////////////////////////////////////////////////////////////////////////////////////
//  Initialisation.
//////////////////////////////////////////////////////////////////////////////////////////

Action::Action() : src(-1), dst(-1), srcTransform(Element::null), dstTransform(Element::null) {}
Action::Action(int src, int dst, Element srcTransform, Element dstTransform) : 
    src(src), dst(dst),
    srcTransform(srcTransform), dstTransform(dstTransform) {
}

bool Action::SrcValid() const {
    return src > -1 && srcTransform != Element::null;
}

bool Action::DstValid() const {
    return dst > -1 && dstTransform != Element::null;
}

bool Action::IsValid() const {
    return SrcValid() && DstValid();
}

SandWorld::SandWorld(int width, int height) : 
    width(width), height(height), 
    queuedMoves(), queuedActions(),
    maxDst(-1),
    chunks(10, 10, 50) { // TODO: Don't hardcode chunk dimensions.
    grid.resize(width * height);
}

void SandWorld::InitProperties() {
    properties.Insert(std::move(std::make_unique<Sand>()));
    properties.Insert(std::move(std::make_unique<Stone>()));
    properties.Insert(std::move(std::make_unique<Water>()));
    properties.Insert(std::move(std::make_unique<Fire>()));
    properties.Insert(std::move(std::make_unique<Wood>()));
    properties.Insert(std::move(std::make_unique<Smoke>()));
}

void SandWorld::InitCells() {
    for (int i = 0; i < grid.size(); i++) {
        Cell newCell {i % width, i / width, &properties};
        // newCell.redraw = true;
        grid[i] = newCell;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Access Functions.
//////////////////////////////////////////////////////////////////////////////////////////

Cell& SandWorld::GetCell(int index) {
    return grid.at(index);
}

Cell& SandWorld::GetCell(int x, int y) {
    return GetCell(ToIndex(x, y));
}

Cell& SandWorld::GetCell(sf::Vector2i p) {
    return GetCell(ToIndex(p.x, p.y));
}

ElementProperties& SandWorld::GetProperties(Cell &cell) {
    return properties.Get(cell.elementId);
}

ElementProperties& SandWorld::GetProperties(int index) {
    return properties.Get(GetCell(index).elementId);
}

ElementProperties& SandWorld::GetProperties(int x, int y) {
    return properties.Get(GetCell(x, y).elementId);
}

ElementProperties& SandWorld::GetProperties(sf::Vector2i p) {
    return properties.Get(GetCell(p).elementId);
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Setting functions.
//////////////////////////////////////////////////////////////////////////////////////////

void SandWorld::SetCell(int index, Element elementId) {
    sf::Vector2i coords {ToCoords(index)};
    GetCell(index).Assign(elementId, coords.x, coords.y);
    chunks.KeepContainingAlive(coords.x, coords.y);
}

void SandWorld::SetCell(int x, int y, Element elementId) {
    GetCell(x, y).Assign(elementId, x, y);
    chunks.KeepContainingAlive(x, y);
}

void SandWorld::SetArea(int x, int y, int w, int h, Element elementId) {
    if (x < 0) {
        w = x + w;
        x = 0;
    } else if (x + w > width) {
        w = width - x;
    }

    if (y < 0) {
        h = y + h;
        y = 0;
    } else if (y + h > height) {
        h = height - y;
    }

    for (int yi = y; yi < y + h; yi++) {
        for (int xi = x; xi < x + w; xi++) {
            SetCell(xi, yi, elementId);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Moving cells.
//////////////////////////////////////////////////////////////////////////////////////////

void SandWorld::MoveCell(int pFrom, int pTo) {
    queuedMoves.emplace_back(pFrom, pTo);
}

void SandWorld::ConsolidateMoves() {
    if (queuedMoves.size() == 0) return;

    // Remove moves that have had their destination filled between frames.
    // TODO. Will probably be needed for threading?

    // Sort the queued moves by destination.
    std::sort(queuedMoves.begin(), queuedMoves.end(), 
        [](const std::pair<int, int> &a, const std::pair<int, int> &b) { return a.second < b.second; }
    );
    
    // As the vector is sorted, this end object is guaranteed to be different to any other element.
    queuedMoves.emplace_back(queuedMoves[0].first - 1, queuedMoves[0].second - 1);

    int iStart {0};

    for (int i = 0; i < queuedMoves.size() - 1; ++i) {
        std::pair<int, int> &move       {queuedMoves.at(i)};
        std::pair<int, int> &nextMove   {queuedMoves.at(i + 1)};

        if (move.second != nextMove.second) {
            int iRand {iStart + QuickRandInt(i - iStart)};
            
            int src {queuedMoves[iRand].first};
            int dst {queuedMoves[iRand].second};
            // Perform the randomly-selected move from the competing moves group.
            Swap(src, dst);
            sf::Vector2i srcCoords {ToCoords(src)};
            sf::Vector2i dstCoords {ToCoords(dst)};
            chunks.KeepContainingAlive(srcCoords.x, srcCoords.y);
            chunks.KeepContainingAlive(dstCoords.x, dstCoords.y);
            // chunks.KeepNeighbourAlive(srcCoords.x, srcCoords.y);

            iStart = i + 1;
        }
    }

    queuedMoves.clear();
}

void SandWorld::Swap(int i1, int i2) {
    std::swap(grid[i1], grid[i2]);

    grid[i1].redraw = true;
    grid[i2].redraw = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Applying actions.
//////////////////////////////////////////////////////////////////////////////////////////

void SandWorld::Act(int src, int dst, Element srcTransform, Element dstTransform) {
    // Ensures that an "invalid" dst still can be sorted when consolidated.
    if (dst == -1) {
        dst = maxDst + 1;
        maxDst = dst;
    }
    queuedActions.emplace_back(src, dst, srcTransform, dstTransform);
}

void SandWorld::ConsolidateActions() {
    if (queuedActions.size() == 0) return;

    // Sort the queued actions by destination.
    std::sort(queuedActions.begin(), queuedActions.end(), 
        [](const Action &a, const Action &b) { return a.dst < b.dst; }
    );

    // Used to catch the final action. 
    Action firstActon {queuedActions[0]};
    queuedActions.emplace_back(firstActon.src - 1, firstActon.dst - 1,  firstActon.srcTransform, firstActon.dstTransform);

    int iStart {0};

    for (int i = 0; i < queuedActions.size() - 1; ++i) {
        Action *move       {&queuedActions.at(i)};
        Action *nextMove   {&queuedActions.at(i + 1)};

        if (move->dst != nextMove->dst) {
            int iRand {iStart + QuickRandInt(i - iStart)};
            
            *move = queuedActions[iRand];
            // Perform the randomly-selected action from the competing actions group.
            if (move->SrcValid()) {
                SetCell(move->src, move->srcTransform);
                sf::Vector2i srcCoords {ToCoords(move->src)};
                chunks.KeepContainingAlive(srcCoords.x, srcCoords.y);
                GetCell(move->src).redraw = true;
            }
            if (move->DstValid()) {
                SetCell(move->dst, move->dstTransform);
                sf::Vector2i dstCoords {ToCoords(move->dst)};
                chunks.KeepContainingAlive(dstCoords.x, dstCoords.y);
                GetCell(move->dst).redraw = true;
            }
        
            iStart = i + 1;
        }
    }

    maxDst = -1;
    queuedActions.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Querying the grid.
//////////////////////////////////////////////////////////////////////////////////////////

bool SandWorld::IsEmpty(int x, int y) {
    if (!InBounds(x, y)) return false;

    if (GetCell(x, y).elementId == Element::air) {
        return true;
    }

    return false;
}

bool SandWorld::IsEmpty(sf::Vector2i p) {
    return IsEmpty(p.x, p.y);
}

bool SandWorld::InBounds(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

bool SandWorld::InBounds(sf::Vector2i p) const {
    return InBounds(p.x, p.y);
}

sf::Vector2i SandWorld::PathEmpty(sf::Vector2i start, sf::Vector2i end) {
    sf::Vector2i dst {start};
    Lerp lerp(start, end);
    for (sf::Vector2i check : lerp) {
        if (IsEmpty(check)) {
            dst = check;
        } else {
            break;
        }
    }

    return dst;
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Helper functions.
//////////////////////////////////////////////////////////////////////////////////////////

size_t SandWorld::Size() const {
    return grid.size();
}

size_t SandWorld::PropertiesSize() const {
    return properties.Size();
}

int SandWorld::ToIndex(int x, int y) const {
    return x + y * width;
}

int SandWorld::ToIndex(sf::Vector2i p) const {
    return ToIndex(p.x, p.y);
}

sf::Vector2i SandWorld::ToCoords(int index) const {
    int y {index / width};
    int x {index % width};

    return sf::Vector2i(x, y);
}
