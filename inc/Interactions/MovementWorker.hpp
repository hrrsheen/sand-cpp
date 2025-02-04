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
public:
    MovementWorker(roomID_t id, SandWorld &_world, SandRoom *_room, float _dt);

    bool PerformMovement(sf::Vector2i p, CellState &cell, ConstProperties &prop);
    void ConsolidateMovement();

private:
    bool MoveCell   (sf::Vector2i p, CellState &cell, ConstProperties &constProp);
    bool SpreadCell (sf::Vector2i p, CellState &cell, ConstProperties &constProp);

    //////// Movement functions ////////
    bool FloatDown      (sf::Vector2i p);
    bool FloatUp        (sf::Vector2i p);
    bool FallDown       (sf::Vector2i p);

    bool SpreadDownSide (sf::Vector2i p);
    bool SpreadUpSide   (sf::Vector2i p);
    bool SpreadSide     (sf::Vector2i p);
};

#endif