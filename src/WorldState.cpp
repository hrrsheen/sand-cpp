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
    if (dt > 1 / 60.f) dt = 1 / 60.f; // DEBUG: Possibly remove this.
    WorldState::dt = dt;
    for (int ci = 0; ci < world.chunks.Size(); ci++) {
        Chunk &chunk {world.chunks.GetChunk(ci)};
        // ChunkBounds area {world.chunks.GetBounds(i)};
        if (!world.chunks.IsActive(ci)) continue;
        if (world.chunks.IsAwake(ci))   world.chunks.UpdateChunk(ci);

        bool chunkActive {false}; // Tracks whether the chunk still requires simulating.
        for (int y = chunk.yMin; y < chunk.yMax; y++) {
            // Process each row.
            // Alternate processing rows left to right and right to left.
            int dir {y % 2};
            int endpoints[] {chunk.xMin, chunk.xMax - 1};
            int start   {endpoints[1 - dir]};
            int end     {endpoints[dir]};
            dir = 2 * dir - 1;
            end += dir;
            for (int x = start; x != end; x += dir) {
                Cell &cell {world.GetCell(x, y)};

                // Apply the simulation.
                if (ApplyRules(cell, sf::Vector2i(x, y))) {
                    world.chunks.KeepContainingAlive(x, y);
                    world.chunks.KeepNeighbourAlive(x, y);
                    chunkActive |= true;
                }
            }
        }

        world.chunks.Set(ci, chunkActive);
    }

    world.ConsolidateActions();
    world.ConsolidateMoves();
}

void WorldState::Draw(Screen &screen) {
    screen.clear();
    for (int i = 0; i < world.Size() - 1; i++) {
        Cell &cell {world.GetCell(i)};
        if (cell.redraw) {
            sf::Vector2i coords {world.ToCoords(i)};
            screen.UpdateCell(coords.x, coords.y, cell.Colour());
            cell.redraw = false;
        }
    }
    screen.Draw();
}

bool WorldState::ApplyRules(Cell &cell, sf::Vector2i p) {
    ElementProperties &properties {cell.Properties()};
    
    if      (ActionCell(cell, properties, p)) { return true; }  // Act on other cells.
    else if (  MoveCell(cell, properties, p)) { return true; }  // Apply movement behaviours (falling, floating, etc).
    else if (SpreadCell(cell, properties, p)) { return true; }  // Apply spreading behaviour.

    return false;
}


bool WorldState::MoveCell(Cell &cell, ElementProperties &properties, sf::Vector2i p) {
    if (world.IsEmpty(p.x, p.y)) return false;
    if (properties.moveBehaviour == MoveType::NONE) return false;

    if      (properties.moveBehaviour == MoveType::FLOAT_DOWN)  { return properties.FloatDown   (p, cell, world);     }
    else if (properties.moveBehaviour == MoveType::FLOAT_UP)    { return properties.FloatUp     (p, cell, world);     }
    else if (properties.moveBehaviour == MoveType::FALL_DOWN)   { return properties.FallDown    (p, cell, world, dt); }

    return false;
}

bool WorldState::SpreadCell(Cell &cell, ElementProperties &properties, sf::Vector2i p) {
    if (world.IsEmpty(p.x, p.y)) return false;
    if (properties.spreadBehaviour == SpreadType::NONE) return false;

    if      (properties.spreadBehaviour & SpreadType::DOWN_SIDE && properties.SpreadDownSide(p, cell, world)) { return true; } 
    else if (properties.spreadBehaviour & SpreadType::UP_SIDE   && properties.SpreadUpSide  (p, cell, world)) { return true; } 
    else if (properties.spreadBehaviour & SpreadType::SIDE      && properties.SpreadSide    (p, cell, world)) { return true; }

    return false;
}

bool WorldState::ActionCell(Cell &cell, ElementProperties &properties, sf::Vector2i p) {
    if (world.IsEmpty(p.x, p.y)) return false;

    if (properties.ActUponSelf(p, cell, world, dt)) { 
        cell.redraw = true;
        return true; 
    } else if (properties.ActUponNeighbours(p, cell, world, dt)) { 
        cell.redraw = true;
        return true; 
    }
}
