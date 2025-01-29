#ifndef INTERACTIONS_ACTION_WORKER_HPP
#define INTERACTIONS_ACTION_WORKER_HPP

#include "Cell.hpp"
#include "Chunks.hpp"
#include "Elements/Names.hpp"
#include "Interactions/InteractionWorker.hpp"
#include "Interactions/ParticleWorker.hpp"
#include "SandRoom.hpp"
#include "SandWorld.hpp"
#include <vector>
#include <unordered_set>

class ActionWorker : public InteractionWorker {
    using cached_points = std::unordered_set<sf::Vector2i, Vector2iHash>;
private:
    ParticleWorker &particles;
    ElementProperties &properties;
    Cells &grid;

public:
    ActionWorker(roomID_t id, SandWorld &_world, SandRoom *_room, ParticleWorker &particles);

    bool PerformActions(CellState &cell, ConstProperties &constProp, sf::Vector2i p);
    void ConsolidateActions();

private:
    bool ActOnSelf    (CellState &cell, ConstProperties &constProp, sf::Vector2i p);
    bool ActOnOther   (CellState &cell, ConstProperties &constProp, sf::Vector2i p);

    //////// Element-specific functions ////////
    // Solid
    bool SandActOnOther     (CellState &cell, ConstProperties &constProp, sf::Vector2i p);
    // Liquid
    bool WaterActOnOther    (CellState &cell, ConstProperties &constProp, sf::Vector2i p);
    // Gas
    bool FireActOnSelf      (CellState &cell, ConstProperties &constProp, sf::Vector2i p);
    bool FireActOnOther     (CellState &cell, ConstProperties &constProp, sf::Vector2i p);

    bool SmokeActOnSelf     (CellState &cell, ConstProperties &constProp, sf::Vector2i p);

    bool SparkActOnSelf     (CellState &cell, ConstProperties &constProp, sf::Vector2i p);

    bool ExplosionActOnSelf (CellState &cell, ConstProperties &constProp, sf::Vector2i p);
    bool ExplosionActOnOther(CellState &cell, ConstProperties &constProp, sf::Vector2i p);

    //////// Helpers for action functions ////////
    // Creates an explosion path from pCentre to pRadius.
    void ExplodeRadius(sf::Vector2i pCentre, sf::Vector2i pRadius, float force, cached_points &cachedCells);
};

#endif