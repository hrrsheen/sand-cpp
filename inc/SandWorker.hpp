#ifndef SANDWORKER_HPP
#define SANDWORKER_HPP

#include "Cell.hpp"
#include "Elements/Names.hpp"
#include "Elements/ElementProperties.hpp"
#include "SandRoom.hpp"
#include "SandWorld.hpp"
#include "Interactions/MovementWorker.hpp"
#include "Interactions/ActionWorker.hpp"
#include <utility>

class SandWorker {
private:
    SandRoom* const room;

    ElementProperties &properties;

    MovementWorker movement;
    ActionWorker actions;
    

    float dt; // The time between frames.

public:
    SandWorker(roomID_t id, SandWorld &_world, SandRoom *_room);

    // Performs one iteration of the simulation.
    void Step(float dt);

private:
    // Performs one step in the simulation of the given chunk.
    void SimulateChunk(Chunk &chunk);

    // Returns true if the cell has performed some action.
    bool ApplyRules(sf::Vector2i p);

    bool ActionCell (CellState &cell, ConstProperties &constProp, sf::Vector2i p);
};

#endif