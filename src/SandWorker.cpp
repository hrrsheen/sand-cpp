#include "Constants.hpp"
#include "SandWorker.hpp"

SandWorker::SandWorker(roomID_t id, SandWorld &_world, SandRoom *_room) :
    movement(id, _world, _room), actions(id, _world, _room), 
    room(_room), properties(_world.properties), dt(0.f) {}

//////////////////////////////////////////////////////////////////////////////////////////
//  Simulation.
//////////////////////////////////////////////////////////////////////////////////////////

void SandWorker::Step(float _dt) {
    if (_dt > 1 / 60.f) _dt = 1 / 60.f; // DEBUG: Possibly remove this.
    movement.SetDeltaTime(_dt);
    actions.SetDeltaTime(_dt);
    for (int ci = 0; ci < room->chunks.Size(); ++ci) {
        Chunk &chunk {room->chunks.GetChunk(ci)};
        room->chunks.UpdateChunk(ci);

        SimulateChunk(chunk);
    }

    actions.ConsolidateActions();
    movement.ConsolidateMovement();
}

void SandWorker::SimulateChunk(Chunk &chunk) {
    for (int y = chunk.yMin; y < chunk.yMax; ++y) { // Inactive chunks will have yMin > yMax.
        // Process each row.
        // Alternate processing rows left to right and right to left.
        int dir {y % 2};
        int endpoints[] {chunk.xMin, chunk.xMax - 1};
        int start   {endpoints[1 - dir]};
        int end     {endpoints[dir]};
        dir = 2 * dir - 1;
        end += dir;
        for (int x = start; x != end; x += dir) {
            // Apply the simulation.
            if (ApplyRules(sf::Vector2i(x, y))) {
                movement.KeepContainingAlive(x, y);
                movement.KeepNeighbourAlive(x, y);
            }
        }
    }
}

bool SandWorker::ApplyRules(sf::Vector2i p) {
    if (room->IsEmpty(p.x, p.y)) return false;

    CellState &cell {room->GetCell(p)};
    ConstProperties &prop {properties.constants[cell.id]};
    
    if      (  actions.PerformActions(cell, prop, p)) { return true; }  // Act on other cells.
    else if (movement.PerformMovement(cell, prop, p)) { return true; }  // Move the cell.

    return false;
}