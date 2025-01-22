#ifndef INTERACTIONS_MOVEMENT_WORKER_HPP
#define INTERACTIONS_MOVEMENT_WORKER_HPP

// #include "Elements/Elements.hpp"
#include "Cell.hpp"
#include "Chunks.hpp"
#include "SandRoom.hpp"
#include "SandWorld.hpp"
#include "Interactions/InteractionWorker.hpp"
#include <SFML/System/Vector2.hpp>
#include <vector>

using roomID_t = int;

class MovementWorker : public InteractionWorker {
    using IW = InteractionWorker;
private:
    float dt;

public:
    MovementWorker(roomID_t id, SandWorld &_world, SandRoom *_room);

    void SetDeltaTime(float _dt);

    bool PerformMovement(CellState &cell, ConstProperties &prop, sf::Vector2i p);
    void ConsolidateMovement();

private:
    bool MoveCell   (CellState &cell, ConstProperties &constProp, sf::Vector2i p);
    bool SpreadCell (CellState &cell, ConstProperties &constProp, sf::Vector2i p);

    //////// Movement functions ////////
    bool FloatDown      (sf::Vector2i p);
    bool FloatUp        (sf::Vector2i p);
    bool FallDown       (sf::Vector2i p, float dt);

    bool SpreadDownSide (sf::Vector2i p);
    bool SpreadUpSide   (sf::Vector2i p);
    bool SpreadSide     (sf::Vector2i p);

    // Helpers for movement
    sf::Vector2i AccelerationDistance(CellState &cell, sf::Vector2i p);
};

#endif