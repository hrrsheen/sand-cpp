#ifndef INTERACTIONS_INTERACTION_WORKER_HPP
#define INTERACTIONS_INTERACTION_WORKER_HPP

#include "Cell.hpp"
#include "Chunks.hpp"
#include "SandRoom.hpp"
#include "SandWorld.hpp"
#include <SFML/System/Vector2.hpp>

roomID_t BoolToID(roomID_t id, bool valid);

class InteractionWorker {
public:
    roomID_t    thisID;
    SandRoom*   room;
    SandWorld   &world;

public:
    InteractionWorker(roomID_t id, SandWorld &_world, SandRoom *_room);

    void KeepContainingAlive(int x, int y);
    void KeepNeighbourAlive(int x, int y);
protected:

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
};

#endif