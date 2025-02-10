#ifndef SANDWORKER_HPP
#define SANDWORKER_HPP

#include "Cell.hpp"
#include "Elements/Names.hpp"
#include "Elements/ElementProperties.hpp"
#include "SandRoom.hpp"
#include "SandWorld.hpp"
#include "WorldDelegate.hpp"
#include <utility>

class SandWorker {
private:
    WorldDelegate del;

    float dt;

public:
    SandWorker(roomID_t id, SandWorld &world, SandRoom *room, float _dt);

    // Performs one iteration of the simulation.
    void UpdateRoom();

private:
    // Performs one step in the simulation of the given chunk.
    void SimulateChunk(Chunk &chunk);

    // Returns true if the cell has performed some action.
    bool SimulateCell(sf::Vector2i p);

    ///////////////////////// Cell Movement ////////////////////////
    bool PerformMovement(sf::Vector2i p, CellState &cell, ConstProperties &prop);

    bool MoveCell   (sf::Vector2i p, CellState &cell, ConstProperties &constProp);
    bool SpreadCell (sf::Vector2i p, CellState &cell, ConstProperties &constProp);

    //////// Movement functions ////////
    bool FloatDown      (sf::Vector2i p);
    bool FloatUp        (sf::Vector2i p);
    bool FallDown       (sf::Vector2i p);

    bool SpreadDownSide (sf::Vector2i p);
    bool SpreadUpSide   (sf::Vector2i p);
    bool SpreadSide     (sf::Vector2i p);

    ///////////////////////// Cell Interactions ////////////////////////
    bool PerformActions(sf::Vector2i p, CellState &cell, ConstProperties &constProp);

    ///////////////////////// Particle Simulation ////////////////////////
    // Updates the position of each particle in the delegate's room. Will convert particles to cells upon collisions.
    void ProcessParticles();

    // Converts a particle to a cell of its containing ID.
    void ParticleToCell(size_t index);
};

#endif