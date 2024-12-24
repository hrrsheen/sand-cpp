#include "Helpers.hpp"
#include "WorldState.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <iostream>

WorldState::WorldState(int width, int height) : 
    brush{Element::air, 5},
    world(width, height) {
    world.InitProperties();
    world.InitCells();
}

void WorldState::Step(float dt) {
    for (int i = 0; i < world.chunks.Size(); i++) {
        ChunkBounds area {world.chunks.GetBounds(i)};
        if (!world.chunks.IsActive(i)) continue;

        bool chunkActive {false}; // Tracks whether the chunk still requires simulating.
        for (int y = area.y; y < area.y + area.size; y++) {
            // Process each row.
            // Alternate processing rows left to right and right to left.
            int dir {y % 2};
            int endpoints[] {area.x, area.x + area.size - 1};
            int start   {endpoints[1 - dir]};
            int end     {endpoints[dir]};
            dir = 2 * dir - 1;
            end += dir;
            for (int x = start; x != end; x += dir) {
                Cell &cell {world.GetCell(x, y)};

                // Apply the simulation.
                ApplyRules(cell, sf::Vector2i(x, y));
            }
        }
    }

    world.ConsolidateActions();
}

void WorldState::draw(Screen &screen) {
    screen.clear();
    for (int i = 0; i < world.Size() - 1; i++) {
        Cell &cell {world.GetCell(i)};
        if (cell.redraw) {
            sf::Vector2i xy {world.ToCoords(i)};
            screen.updateCell(xy.x, xy.y, cell.colour);
            cell.redraw = false;
        }
    }
    screen.draw();
}


void WorldState::wakeAdjacentChunks(Cell &cell) {
    ChunkBounds area {world.chunks.GetContainingBounds(cell.p.x, cell.p.y)};
    sf::Vector2i chunk {world.chunks.ContainingChunk(cell.p.x, cell.p.y)};
    // Wake the left-hand chunk.
    if (chunk.x > 0 && cell.p.x == area.x) {
        world.chunks.Set(chunk.x - 1, chunk.y, true);
    }
    // Wake the right-hand chunk.
    if (chunk.x < world.chunks.width - 1 && cell.p.x == area.x + area.size - 1) {
        world.chunks.Set(chunk.x + 1, chunk.y, true);
    }
}

void WorldState::ApplyRules(Cell &cell, sf::Vector2i p) {
    ElementProperties &properties {cell.Properties()};
    
    if (MoveCell(cell, properties, p)) {}          // Apply movement behaviours (falling, floating, etc).
    else if (SpreadCell(cell, properties, p)) {}   // Apply spreading behaviour.
    else if (ActionCell(cell, properties, p)) {}   // Act on other cells.
}


bool WorldState::MoveCell(Cell &cell, ElementProperties &properties, sf::Vector2i p) {
    if (properties.moveBehaviour == MoveType::NONE) return true;

    if (properties.moveBehaviour == MoveType::FLOAT_DOWN) {
        return FloatDown(p);
    } else if (properties.moveBehaviour == MoveType::FLOAT_UP) {
        return FloatUp(p);
    }

    return false;
}

bool WorldState::SpreadCell(Cell &cell, ElementProperties &properties, sf::Vector2i p) {
    if (properties.spreadBehaviour == SpreadType::NONE) return true;

    if (properties.spreadBehaviour & SpreadType::DOWN_SIDE) {
        return SpreadDownSide(p);
    } else if (properties.spreadBehaviour & SpreadType::UP_SIDE) {
        return SpreadUpSide(p);
    } else if (properties.spreadBehaviour & SpreadType::SIDE) {
        return SpreadSide(p);
    }

    return false;
}

bool WorldState::ActionCell(Cell &cell, ElementProperties &properties, sf::Vector2i p) {
    return true;
}

bool WorldState::FloatDown(sf::Vector2i p) {
    sf::Vector2i queryPos(p.x, p.y - 1);
    if (world.IsEmpty(queryPos.x, queryPos.y)) {
        world.MoveCell(world.ToIndex(p.x, p.y), world.ToIndex(queryPos.x, queryPos.y));
        return true;
    }

    return false;
}

bool WorldState::FloatUp(sf::Vector2i p) {
    sf::Vector2i queryPos(p.x, p.y - 1);
    if (world.IsEmpty(queryPos.x, queryPos.y)) {
        world.MoveCell(world.ToIndex(p.x, p.y), world.ToIndex(queryPos.x, queryPos.y));
        return true;
    }

    return false;
}

bool WorldState::SpreadDownSide(sf::Vector2i p) {
    sf::Vector2i leftPos    {p + sf::Vector2i( 1, -1)};
    sf::Vector2i rightPos   {p + sf::Vector2i(-1, -1)};

    bool downLeft   {world.IsEmpty( leftPos.x,  leftPos.y)};
    bool downRight  {world.IsEmpty(rightPos.x, rightPos.y)};

    if (downLeft && downRight) {
        downLeft    = QuickRandInt(100) > 49;
        downRight   = !downLeft;
    }

    if (downLeft) { 
        world.MoveCell(world.ToIndex(p.x, p.y), world.ToIndex( leftPos.x,  leftPos.y));
    } else if (downRight) {
        world.MoveCell(world.ToIndex(p.x, p.y), world.ToIndex(rightPos.x, rightPos.y));
    }

    return downLeft || downRight;
}

bool WorldState::SpreadUpSide(sf::Vector2i p) {
    sf::Vector2i leftPos    {p + sf::Vector2i( 1, 1)};
    sf::Vector2i rightPos   {p + sf::Vector2i(-1, 1)};

    bool upLeft   {world.IsEmpty( leftPos.x,  leftPos.y)};
    bool upRight  {world.IsEmpty(rightPos.x, rightPos.y)};

    if (upLeft && upRight) {
        upLeft    = QuickRandInt(100) > 49;
        upRight   = !upLeft;
    }

    if (upLeft) { 
        world.MoveCell(world.ToIndex(p.x, p.y), world.ToIndex( leftPos.x,  leftPos.y));
    } else if (upRight) {
        world.MoveCell(world.ToIndex(p.x, p.y), world.ToIndex(rightPos.x, rightPos.y));
    }

    return upLeft || upRight;
}

bool WorldState::SpreadSide(sf::Vector2i p) {
    sf::Vector2i leftPos    {p + sf::Vector2i( 1, 0)};
    sf::Vector2i rightPos   {p + sf::Vector2i(-1, 0)};

    bool left   {world.IsEmpty( leftPos.x,  leftPos.y)};
    bool right  {world.IsEmpty(rightPos.x, rightPos.y)};

    if (left && right) {
        left    = QuickRandInt(100) > 49;
        right   = !left;
    }

    if (left) { 
        world.MoveCell(world.ToIndex(p.x, p.y), world.ToIndex( leftPos.x,  leftPos.y));
    } else if (right) {
        world.MoveCell(world.ToIndex(p.x, p.y), world.ToIndex(rightPos.x, rightPos.y));
    }

    return left || right;
}
