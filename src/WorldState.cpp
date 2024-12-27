#include "Helpers.hpp"
#include "WorldState.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <iostream>

WorldState::WorldState(int width, int height) : 
    brush{Element::air, 5},
    world(width, height),
    dt(0.f) {
    world.InitProperties();
    world.InitCells();
}

void WorldState::Step(float dt) {
    WorldState::dt = dt;
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
                chunkActive |= ApplyRules(cell, sf::Vector2i(x, y));
            }
        }

        world.chunks.Set(i, chunkActive);
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

bool WorldState::ApplyRules(Cell &cell, sf::Vector2i p) {
    ElementProperties &properties {cell.Properties()};
    
    if      (  MoveCell(cell, properties, p)) { return true; }  // Apply movement behaviours (falling, floating, etc).
    else if (SpreadCell(cell, properties, p)) { return true; }  // Apply spreading behaviour.
    else if (ActionCell(cell, properties, p)) { return true; }  // Act on other cells.

    return false;
}


bool WorldState::MoveCell(Cell &cell, ElementProperties &properties, sf::Vector2i p) {
    if (world.IsEmpty(p.x, p.y)) return false;
    if (properties.moveBehaviour == MoveType::NONE) return false;

    if (properties.moveBehaviour == MoveType::FLOAT_DOWN) {
        return FloatDown(p);
    } else if (properties.moveBehaviour == MoveType::FLOAT_UP) {
        return FloatUp(p);
    } else if (properties.moveBehaviour == MoveType::ACCELERATE_DOWN) {
        return AccelerateDown(p, cell);
    }

    return false;
}

bool WorldState::SpreadCell(Cell &cell, ElementProperties &properties, sf::Vector2i p) {
    if (world.IsEmpty(p.x, p.y)) return false;
    if (properties.spreadBehaviour == SpreadType::NONE) return false;

    if      (properties.spreadBehaviour & SpreadType::DOWN_SIDE && SpreadDownSide(p)) { return true; } 
    else if (properties.spreadBehaviour & SpreadType::UP_SIDE   && SpreadUpSide(p)) { return true; } 
    else if (properties.spreadBehaviour & SpreadType::SIDE      && SpreadSide(p, properties)) { return true; }

    return false;
}

bool WorldState::ActionCell(Cell &cell, ElementProperties &properties, sf::Vector2i p) {
    if (world.IsEmpty(p.x, p.y)) return false;
    return false;
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
    return SpreadUpSide(p);
}

bool WorldState::AccelerateDown(sf::Vector2i p, Cell &cell) {
    cell.ApplyAcceleration(sf::Vector2f(0.f, -ACCELERATION), dt);
    sf::Vector2i dp {0, static_cast<int>(cell.velocity.y * dt)};
    
    sf::Vector2i dst {p};
    Lerp lerp(p + sf::Vector2i(0, -1), p + dp);
    for (sf::Vector2i check : lerp) {
        if (world.IsEmpty(check.x, check.y)) {
            dst = check;
        } else {
            break;
        }
    }

    if (dst != p) {
        world.MoveCell(world.ToIndex(p.x, p.y), world.ToIndex(dst.x, dst.y));
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

bool WorldState::SpreadSide(sf::Vector2i p, ElementProperties &properties) {
    sf::Vector2i lookAhead  {1, 0};

    bool left   {world.IsEmpty(p - lookAhead)};
    bool right  {world.IsEmpty(p + lookAhead)};

    if (left && right) {
        left    = QuickRandInt(100) > 49;
        right   = !left;
    }

    if (left) {
        sf::Vector2i dst {world.PathEmpty(p - lookAhead, p - properties.SpreadRate() * lookAhead)};
        world.MoveCell(world.ToIndex(p.x, p.y), world.ToIndex( dst.x,  dst.y));
    } else if (right) {
        sf::Vector2i dst {world.PathEmpty(p + lookAhead, p + properties.SpreadRate() * lookAhead)};
        world.MoveCell(world.ToIndex(p.x, p.y), world.ToIndex( dst.x,  dst.y));
    }

    return left || right;
}
