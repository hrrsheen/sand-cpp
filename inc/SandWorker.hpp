#ifndef SANDWORKER_HPP
#define SANDWORKER_HPP

#include "Cell.hpp"
#include "Elements/Names.hpp"
#include "Elements/ElementProperties.hpp"
#include "SandRoom.hpp"
#include "SandWorld.hpp"
#include <utility>

class SandWorker {
private:
    roomID_t    thisID;
    SandWorld   &world;
    SandRoom* const room;

    ElementProperties &properties;

    float dt; // The time between frames.

public:
    SandWorker(roomID_t id, SandWorld &_world, SandRoom *_room);

    // Performs one iteration of the simulation.
    void Step(float dt);

private:
    CellState &GetCell(int x, int y);
    CellState &GetCell(sf::Vector2i p);
    size_t CellIndex(sf::Vector2i);
    void SetCell(int x, int y, Element id);

    roomID_t ContainingRoomID(sf::Vector2i p);
    SandRoom *GetRoom(roomID_t id);

    // Returns the containing room's ID if the given point is empty, -1 otherwise.
    roomID_t IsEmpty(int x, int y);
    roomID_t IsEmpty(sf::Vector2i p);
    std::pair<roomID_t, sf::Vector2i> PathEmpty(sf::Vector2i start, sf::Vector2i end);

    //////// Simulation functions ////////
    void SimulateChunk(Chunk &chunk);

    // Returns true if the cell has performed some action.
    bool ApplyRules(sf::Vector2i p);

    bool MoveCell   (CellState &cell, ConstProperties &constProp, sf::Vector2i p);
    bool SpreadCell (CellState &cell, ConstProperties &constProp, sf::Vector2i p);
    bool ActionCell (CellState &cell, ConstProperties &constProp, sf::Vector2i p);

    //////// Movement functions ////////
    bool FloatDown      (sf::Vector2i p);
    bool FloatUp        (sf::Vector2i p);
    bool FallDown       (sf::Vector2i p, float dt);

    bool SpreadDownSide (sf::Vector2i p);
    bool SpreadUpSide   (sf::Vector2i p);
    bool SpreadSide     (sf::Vector2i p);
};

#endif