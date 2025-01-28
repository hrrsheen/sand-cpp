#ifndef INTERACTIONS_ACTION_WORKER_HPP
#define INTERACTIONS_ACTION_WORKER_HPP

#include "Cell.hpp"
#include "Chunks.hpp"
#include "Elements/Names.hpp"
#include "Interactions/InteractionWorker.hpp"
#include "SandRoom.hpp"
#include "SandWorld.hpp"
#include <vector>
#include <unordered_set>

class ActionWorker : public InteractionWorker {
    using IW = InteractionWorker;
    using cached_points = std::unordered_set<sf::Vector2i, Vector2iHash>;
private:
    float dt;

    ElementProperties &properties;
    Cells &grid;

public:
    ActionWorker(roomID_t id, SandWorld &_world, SandRoom *_room);

    bool PerformActions(CellState &cell, ConstProperties &constProp, sf::Vector2i p);
    void ConsolidateActions();

private:
    bool ActOnSelf    (size_t self, float dt);
    bool ActOnOther   (size_t self, size_t other, SandRoom *otherRoom, float dt);

    //////// Element-specific functions ////////
    // Solid
    bool SandActOnOther(size_t self, size_t other, SandRoom *otherRoom, float dt);
    // Liquid
    bool WaterActOnOther(size_t self, size_t other, SandRoom *otherRoom, float dt);
    // Gas
    bool FireActOnSelf(size_t self, float dt);
    bool FireActOnOther(size_t self, size_t other, SandRoom *otherRoom, float dt);

    bool SmokeActOnSelf(size_t self, float dt);

    bool SparkActOnSelf(size_t self, float dt);

    void ExplodeRadius(sf::Vector2i pCentre, sf::Vector2i pRadius, float force, cached_points &cachedCells);
    bool ExplosionActOnSelf(size_t self, float dt);
    bool ExplosionActOnOther(size_t self, size_t other, SandRoom *otherRoom, float dt);
};

#endif