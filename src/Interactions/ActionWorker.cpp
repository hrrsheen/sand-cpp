#include "Interactions/ActionWorker.hpp"
#include <cmath>
#include <iostream>
#include <unordered_set>

ActionWorker::ActionWorker(roomID_t id, SandWorld &_world, SandRoom *_room) : 
    InteractionWorker(id, _world, _room), properties(_world.properties), grid(_room->grid) {}

void ActionWorker::SetDeltaTime(float _dt) {
    dt = _dt;
}

bool ActionWorker::PerformActions(CellState &cell, ConstProperties &prop, sf::Vector2i p) {
    int thisIndex {room->ToIndex(p)};
    if (ActOnSelf(thisIndex, dt)) {
        return true;
    }

    bool acted {false};
    for (const sf::Vector2i deltaP : prop.actionSet) {
        sf::Vector2i otherP {p + deltaP};
        roomID_t roomID {ContainingRoomID(otherP)};
        if (VALID_ROOM(roomID)) {
            SandRoom *otherRoom {GetRoom(roomID)};
            if (ActOnOther(thisIndex, otherRoom->ToIndex(otherP), otherRoom, dt)) {
                acted = true;
            }
        }
    }

    return acted;
}

void ActionWorker::ConsolidateActions() {
    if (room->queuedActions.size() == 0) return;

    // Sort the queued actions by destination.
    std::sort(room->queuedActions.begin(), room->queuedActions.end(), 
        [](const std::pair<size_t, Element> &a, const std::pair<size_t, Element> &b) { 
            return a.first < b.first;
        }
    );

    // Used to catch the final action. 
    room->queuedActions.emplace_back(-1, Element::null);

    int iStart {0};

    for (int i = 0; i < room->queuedActions.size() - 1; ++i) {
        std::pair<size_t, Element> move       {room->queuedActions.at(i)};
        std::pair<size_t, Element> nextMove   {room->queuedActions.at(i + 1)};

        if (move.first != nextMove.first) {
            // Perform the randomly-selected action from the competing actions group.
            int iRand {iStart + QuickRandInt(i - iStart)};
            
            size_t iCell {room->queuedActions[iRand].first};
            Element tfID {room->queuedActions[iRand].second};
            grid.Assign(iCell, tfID);

            sf::Vector2i coords {room->ToWorldCoords(iCell)};
            room->chunks.KeepContainingAlive(coords.x, coords.y);
        
            iStart = i + 1;
        }
    }

    room->queuedActions.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
//  High-level action functions.
//////////////////////////////////////////////////////////////////////////////////////////

bool ActionWorker::ActOnSelf(size_t self, float dt) {
    switch(grid.state[self].id) {
        case Element::fire:
            return FireActOnSelf(self, dt);
        case Element::explosion:
            return ExplosionActOnSelf(self, dt);
        case Element::smoke:
            return SmokeActOnSelf(self, dt);
        case Element::spark:
            return SparkActOnSelf(self, dt);
        default:
            return false;
    }
}

bool ActionWorker::ActOnOther(size_t self, size_t other, SandRoom *otherRoom, float dt) {
    switch(grid.state[self].id) {
        case Element::sand:
            return SandActOnOther(self, other, otherRoom, dt);
        case Element::water:
            return WaterActOnOther(self, other, otherRoom, dt);
        case Element::fire:
            return FireActOnOther(self, other, otherRoom, dt);
        default:
            return false;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Specific action functions.
//////////////////////////////////////////////////////////////////////////////////////////

//////////////// Solid interactions ////////////////

bool ActionWorker::SandActOnOther(size_t self, size_t other, SandRoom *otherRoom, float dt) {
    if (otherRoom->GetCell(other).id == Element::fire) {
        room->QueueAction(self, Element::air);
        otherRoom->QueueAction(other, Element::sand);
        return true;
    }

    return false;
}

//////////////// Liquid interactions ////////////////

bool ActionWorker::WaterActOnOther(size_t self, size_t other, SandRoom *otherRoom, float dt) {
    if (otherRoom->GetCell(other).id == Element::fire) {
        room->QueueAction(self, Element::smoke);
        otherRoom->QueueAction(other, Element::water);
        return true;
    }

    return false;
}

//////////////// Gas interactions ////////////////

bool ActionWorker::FireActOnSelf(size_t self, float dt) {
    CellState &thisState {grid.state[self]};
    if (thisState.health <= 0) {
        if (Probability(20))
            room->QueueAction(self, Element::smoke);
        else
            room->QueueAction(self, Element::air);

        return true;
    }
    grid.colour[self] = properties.Colour(Element::fire); // Recolour fire every frame.
    thisState.health -= (500.f + static_cast<float>(QuickRandInt(200))) * dt;

    sf::Vector2i coords {room->ToWorldCoords(self)};
    KeepContainingAlive(coords.x, coords.y);
    return false;   
}

bool ActionWorker::FireActOnOther(size_t self, size_t other, SandRoom *otherRoom, float dt) {
    CellState &otherState {otherRoom->GetCell(other)};
    float flammability {properties.constants[otherState.id].flammability};
    if (flammability > 0.f) {
        grid.state[self].health += flammability * dt;
        if (otherState.health <= 0.f) {
            otherRoom->QueueAction(other, Element::fire);
        } else {
            otherState.health -= flammability * dt;
        }
        return true;
    }
    return false;
}

bool ActionWorker::SmokeActOnSelf(size_t self, float dt) {
    CellState &thisState {grid.state[self]};
    if (thisState.health <= 0) {
        room->QueueAction(self, Element::air);
    }

    thisState.health -= (100.f + static_cast<float>(QuickRandRange(-50, 50))) * dt;

    sf::Vector2i coords {room->ToWorldCoords(self)};
    KeepContainingAlive(coords.x, coords.y);
    return false;
}

bool ActionWorker::SparkActOnSelf(size_t self, float dt) {
    CellState &thisState {grid.state[self]};
    if (thisState.health <= 0) {
        room->QueueAction(self, Element::air);
        return true;
    }

    grid.colour[self] = properties.Colour(Element::spark); // Recolour spark every frame.
    float randFalloff = static_cast<float>(RandInt(5000));
    thisState.health -= (10.f + randFalloff) * dt;

    sf::Vector2i coords {room->ToWorldCoords(self)};
    KeepContainingAlive(coords.x, coords.y);
    return false;   
}

bool ActionWorker::ExplosionActOnSelf(size_t self, float dt) {
    sf::Vector2i centrei {room->ToWorldCoords(self)};
    sf::Vector2f centref {centrei};
    int radius {25};
    sf::Vector2i corners[4] {
        {-radius,  radius},
        { radius,  radius},
        { radius, -radius},
        {-radius, -radius}};
    std::unordered_set<sf::Vector2i, Vector2iHash> cachedCells;
    SandRoom *explosionRoom {room};
    for (int i = 0; i < 4; ++i) {
        sf::Vector2i start  {corners[i]};
        sf::Vector2i end    {corners[(i + 1) % 4]};
        
        // Trace an edge of the square.
        Lerp lerp(start, end);
        for (Lerp::iterator pointIt = lerp.begin(); pointIt != --lerp.end(); ++pointIt) {
            // Find the point on the circle's perimiter to radiate to.
            sf::Vector2f dir {*pointIt};
            dir = math::Normalise(dir) * (radius + 0.5f);
            sf::Vector2i circlePerimiter {math::RoundPoint(centref + dir)};

            float force {100.f};
            for (sf::Vector2i point : Lerp(centrei, circlePerimiter)) {
                if (cachedCells.find(point) != cachedCells.end()) continue;
                cachedCells.insert(point); // Add to the set so we don't repeat actions on this cell.
                
                // Account for explosions crossing rooms.
                if (!explosionRoom->InBounds(point)) {
                    roomID_t newRoomID {ContainingRoomID(point)};
                    if (VALID_ROOM(newRoomID))
                        explosionRoom = GetRoom(newRoomID);
                    else
                        break;
                }

                // Dampen the explosion based on the element hardness.
                force -= GetProperties(point).hardness;
                if (force <= 0.f) {
                    break;
                } else {
                    if (Probability(80))
                        explosionRoom->QueueAction(explosionRoom->ToIndex(point), Element::air);
                    else
                        explosionRoom->QueueAction(explosionRoom->ToIndex(point), Element::spark);
                }
            }
        }
    }
    return true;
}

bool ActionWorker::ExplosionActOnOther(size_t self, size_t other, SandRoom *otherRoom, float dt) {
    return false;
}
