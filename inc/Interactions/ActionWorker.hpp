#ifndef INTERACTIONS_ACTION_WORKER_HPP
#define INTERACTIONS_ACTION_WORKER_HPP

#include "Cell.hpp"
#include "Chunks.hpp"
#include "Elements/Names.hpp"
#include "Interactions/InteractionWorker.hpp"
#include "Interactions/ParticleWorker.hpp"
#include "SandRoom.hpp"
#include "SandWorld.hpp"
#include "Utility/Hashes.hpp"
#include <vector>
#include <unordered_set>

class ActionWorker : public InteractionWorker {
    using cached_points = std::unordered_set<sf::Vector2i, Vector2iHash>;
private:
    ParticleWorker &particles;
    ElementProperties &properties;
    Cells &grid;

public:
    ActionWorker(roomID_t id, SandWorld &_world, SandRoom *_room, ParticleWorker &particles, float _dt);

    bool PerformActions(sf::Vector2i p, CellState &cell, ConstProperties &constProp);
    void ConsolidateActions();

private:
    bool ActOnSelf    (sf::Vector2i p, CellState &cell, ConstProperties &constProp);
    bool ActOnOther   (sf::Vector2i p, CellState &cell, ConstProperties &constProp);

    //////// Element-specific functions ////////
    // Solid
    bool SandActOnOther     (sf::Vector2i p, CellState &cell, ConstProperties &constProp);
    // Liquid
    bool WaterActOnOther    (sf::Vector2i p, CellState &cell, ConstProperties &constProp);
    // Gas
    bool FireActOnSelf      (sf::Vector2i p, CellState &cell, ConstProperties &constProp);
    bool FireActOnOther     (sf::Vector2i p, CellState &cell, ConstProperties &constProp);

    bool SmokeActOnSelf     (sf::Vector2i p, CellState &cell, ConstProperties &constProp);

    bool SparkActOnSelf     (sf::Vector2i p, CellState &cell, ConstProperties &constProp);

    bool ExplosionActOnSelf (sf::Vector2i p, CellState &cell, ConstProperties &constProp);
    bool ExplosionActOnOther(sf::Vector2i p, CellState &cell, ConstProperties &constProp);

    //////// Helpers for action functions ////////
    // Creates an explosion path from pCentre to pRadius.
    void ExplodeRadius(sf::Vector2i pCentre, sf::Vector2i pRadius, float force, cached_points &cachedCells, cached_points &cachedShockwave);
};

#endif