#ifndef INTERACTIONS_ACTION_WORKER_HPP
#define INTERACTIONS_ACTION_WORKER_HPP

#include "Cell.hpp"
#include "Chunks.hpp"
#include "Elements/Names.hpp"
#include "Interactions/InteractionWorker.hpp"
#include "SandRoom.hpp"
#include "SandWorld.hpp"
#include <vector>

class ActionWorker : public InteractionWorker {
    using IW = InteractionWorker;
private:
    float dt;

    ElementProperties &properties;
    Cells &grid;

public:
    ActionWorker(roomID_t id, SandWorld &_world, SandRoom *_room);

    void SetDeltaTime(float _dt);

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

    bool ExplosionActOnSelf(size_t self, float dt);
    bool ExplosionActOnOther(size_t self, size_t other, SandRoom *otherRoom, float dt);
};

#endif