#ifndef INTERACTIONS_INTERACTION_WORKER_HPP
#define INTERACTIONS_INTERACTION_WORKER_HPP

#include "Cell.hpp"
#include "Chunks.hpp"
#include "SandRoom.hpp"
#include "SandWorld.hpp"
#include "Utility/Line.hpp"
#include <SFML/System/Vector2.hpp>

enum PathOpts : uint8_t {
    NO_OPTS = 0b0000,
    SPAWN   = 0b0001, // Spawn new rooms as the path crosses them.
    SKIP    = 0b0010, // Skip the starting point of the path.
};

// Helper functions used by all derived classes
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

    const ConstProperties& GetProperties(int index) const;
    const ConstProperties& GetProperties(sf::Vector2i p) const;

    roomID_t ContainingRoomID(sf::Vector2i p);
    SandRoom *GetRoom(roomID_t id);

    // Returns the containing room's ID if the given point is empty, -1 otherwise.
    roomID_t IsEmpty(int x, int y);
    roomID_t IsEmpty(sf::Vector2i p);

    template <PathOpts Op=PathOpts::NO_OPTS>
    std::pair<roomID_t, sf::Vector2i> PathEmpty(sf::Vector2i start, sf::Vector2i end);
};

template <PathOpts Op>
std::pair<roomID_t, sf::Vector2i> InteractionWorker::PathEmpty(sf::Vector2i start, sf::Vector2i end) {
    SandRoom *checkRoom = room;
    roomID_t  checkID   = thisID, validID   = -1;
    sf::Vector2i dst {start};

    Lerp line {start, end};
    Lerp::iterator lineIt {line.begin()};
    if constexpr (Op & PathOpts::SKIP) {
        ++lineIt;
    }
    for (; lineIt != line.end(); ++lineIt) {
        sf::Vector2i check {*lineIt};

        // If the check position is outside the world borders, no further checks are needed.
        if (world.InBounds(check)) {
            if (!checkRoom->InBounds(check)) checkID = ContainingRoomID(check); // Only need to update the room if we've crossed a border.

            if (VALID_ROOM(checkID)) {
                checkRoom = GetRoom(checkID);
                if (checkRoom->IsEmpty(check)) {
                    dst = check;
                    validID = checkID;
                } else {
                    break;
                }
            } else {
                dst = check;
                if constexpr (Op & PathOpts::SPAWN) {
                    validID = world.SpawnRoom(dst.x, dst.y);
                } else {
                    validID = checkID;
                }
            }
        } else {
            break;
        }
    }

    return {validID, dst};
}

#endif