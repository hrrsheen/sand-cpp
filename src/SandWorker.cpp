#include "Constants.hpp"
#include "SandWorker.hpp"

SandWorker::SandWorker(roomID_t id, SandWorld &_world, SandRoom *_room, float _dt) :
    movement(id, _world, _room, _dt), actions(id, _world, _room, particles, _dt), particles(id, _world, _room, _dt),
    room(_room), properties(_world.properties) {}

//////////////////////////////////////////////////////////////////////////////////////////
//  Simulation.
//////////////////////////////////////////////////////////////////////////////////////////

void SandWorker::Step() {
    particles.ProcessParticles();
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
        int dir = std::abs(y % 2);
        int endpoints[] {chunk.xMin, chunk.xMax - 1};
        int start   = endpoints[1 - dir];
        int end     = endpoints[dir];
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
    
    if      (  actions.PerformActions(p, cell, prop)) { return true; }  // Act on other cells.
    else if (movement.PerformMovement(p, cell, prop)) { return true; }  // Move the cell.

    return false;
}