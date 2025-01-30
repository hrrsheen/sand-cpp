#include "Interactions/ActionWorker.hpp"
#include "Utility/Random.hpp"
#include <cmath>
#include <iostream>

ActionWorker::ActionWorker(roomID_t id, SandWorld &_world, SandRoom *_room, ParticleWorker &_particles) : 
    InteractionWorker(id, _world, _room), particles(_particles), properties(_world.properties), grid(_room->grid) {}

bool ActionWorker::PerformActions(CellState &cell, ConstProperties &prop, sf::Vector2i p) {
    if      (ActOnSelf (cell, prop, p)) { return true; } 
    else if (ActOnOther(cell, prop, p)) { return true; }

    return false;
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

bool ActionWorker::ActOnSelf(CellState &cell, ConstProperties &constProp, sf::Vector2i p) {
    switch(cell.id) {
        case Element::fire:
            return FireActOnSelf(cell, constProp, p);
        case Element::explosion:
            return ExplosionActOnSelf(cell, constProp, p);
        case Element::smoke:
            return SmokeActOnSelf(cell, constProp, p);
        case Element::spark:
            return SparkActOnSelf(cell, constProp, p);
        default:
            return false;
    }
}

bool ActionWorker::ActOnOther(CellState &cell, ConstProperties &constProp, sf::Vector2i p) {
    switch(cell.id) {
        case Element::sand:
            return SandActOnOther(cell, constProp, p);
        case Element::water:
            return WaterActOnOther(cell, constProp, p);
        case Element::fire:
            return FireActOnOther(cell, constProp, p);
        default:
            return false;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Specific action functions.
//////////////////////////////////////////////////////////////////////////////////////////

//////////////// Solid interactions ////////////////

bool ActionWorker::SandActOnOther(CellState &cell, ConstProperties &prop, sf::Vector2i p) {
    sf::Vector2i otherP {p + sf::Vector2i {0, -1}};
    size_t self = room->ToIndex(p);

    roomID_t roomID = ContainingRoomID(otherP);
    if (VALID_ROOM(roomID)) {
        SandRoom *otherRoom = GetRoom(roomID);
        size_t other        = otherRoom->ToIndex(otherP);

        if (otherRoom->GetCell(other).id == Element::fire) {
            room->QueueAction(self, Element::air);
            otherRoom->QueueAction(other, Element::sand);
            return true;
        }
    }

    return false;
}

//////////////// Liquid interactions ////////////////

bool ActionWorker::WaterActOnOther(CellState &cell, ConstProperties &prop, sf::Vector2i p) {
    sf::Vector2i otherP {p + sf::Vector2i {0, -1}};
    size_t self = room->ToIndex(p);

    roomID_t roomID = ContainingRoomID(otherP);
    if (VALID_ROOM(roomID)) {
        SandRoom *otherRoom = GetRoom(roomID);
        size_t other        = otherRoom->ToIndex(otherP);

        if (otherRoom->GetCell(other).id == Element::fire) {
            room->QueueAction(self, Element::smoke);
            otherRoom->QueueAction(other, Element::water);
            return true;
        }
    }

    return false;
}

//////////////// Gas interactions ////////////////

bool ActionWorker::FireActOnSelf(CellState &cell, ConstProperties &prop, sf::Vector2i p) {
    size_t self = room->ToIndex(p);

    if (cell.health <= 0) {
        if (Probability(20))
            room->QueueAction(self, Element::smoke);
        else
            room->QueueAction(self, Element::air);

        return true;
    }
    grid.colour[self] = properties.Colour(Element::fire); // Recolour fire every frame.
    cell.health -= (500.f + static_cast<float>(QuickRandInt(200))) * dt;

    KeepContainingAlive(p.x, p.y);
    return false;   
}

bool ActionWorker::FireActOnOther(CellState &cell, ConstProperties &prop, sf::Vector2i p) {
    size_t self = room->ToIndex(p);

    bool acted = false;
    // Iterate over all cells that the fire can affect.
    for (const sf::Vector2i dp : prop.actionSet) {
        sf::Vector2i otherP {p + dp};
        roomID_t roomID = ContainingRoomID(otherP);
        if (VALID_ROOM(roomID)) {
            SandRoom  *otherRoom = GetRoom(roomID);
            size_t     other     = otherRoom->ToIndex(otherP);
            CellState &otherCell = otherRoom->GetCell(other);

            // Action code.
            float flammability = properties.constants[otherCell.id].flammability;
            if (flammability > 0.f) {
                cell.health += flammability * dt;
                if (otherCell.health <= 0.f)
                    otherRoom->QueueAction(other, Element::fire);
                else
                    otherCell.health -= flammability * dt;

                acted = true;
            }
        }
    }

    return acted;
}

bool ActionWorker::SmokeActOnSelf(CellState &cell, ConstProperties &prop, sf::Vector2i p) {
    size_t self = room->ToIndex(p);

    if (cell.health <= 0) {
        room->QueueAction(self, Element::air);
    }

    cell.health -= (100.f + static_cast<float>(QuickRandRange(-50, 50))) * dt;

    KeepContainingAlive(p.x, p.y);
    return false;
}

bool ActionWorker::SparkActOnSelf(CellState &cell, ConstProperties &prop, sf::Vector2i p) {
    size_t self = room->ToIndex(p);

    if (cell.health <= 0) {
        room->QueueAction(self, Element::air);
        return true;
    }

    grid.colour[self] = properties.Colour(Element::spark); // Recolour spark every frame.

    float randFalloff = static_cast<float>(RandInt(5000));
    cell.health -= (10.f + randFalloff) * dt;

    KeepContainingAlive(p.x, p.y);
    return false;   
}

void ActionWorker::ExplodeRadius(sf::Vector2i pCentre, sf::Vector2i pRadius, float force, cached_points &cachedCells) {
    // Set up the variables for a supercover line.
    int dx      = pRadius.x - pCentre.x,    dy      = pRadius.y - pCentre.y;    // The difference in point positions.
    float nx    = std::abs(dx),             ny      = std::abs(dy);             // The number of grid spaces to move.
    int sgnx    = (dx > 0) - (dx < 0),      sgny    = (dy > 0) - (dy < 0);      // The direction to step.
    SandRoom *explosionRoom {room};

    sf::Vector2i point {pCentre};
    for (int ix = 0, iy = 0; ix < nx || iy < ny; ) {
        if (cachedCells.find(point) == cachedCells.end()) {
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

        if ((0.5 + ix) / nx < (0.5 + iy) / ny) {
            // Take a horizontal step.
            point.x += sgnx;
            ix++;
        } else {
            // Take a vertical step.
            point.y += sgny;
            iy++;
        }
    }
}

bool ActionWorker::ExplosionActOnSelf(CellState &cell, ConstProperties &prop, sf::Vector2i p) {
    float radius {25.5};
    cached_points cachedCells;
    SandRoom *explosionRoom {room};
    float force {100.f};
    for (int h = 0; h <= std::round(radius * std::sqrtf(0.5f)); ++h) {
        int b {static_cast<int>(std::round(std::sqrtf(radius * radius - h * h)))};

        // The circumference calculation can be repeated for each octant.
        ExplodeRadius(p, sf::Vector2i(p.x + b, p.y + h), force, cachedCells);
        ExplodeRadius(p, sf::Vector2i(p.x - b, p.y + h), force, cachedCells);
        ExplodeRadius(p, sf::Vector2i(p.x + b, p.y - h), force, cachedCells);
        ExplodeRadius(p, sf::Vector2i(p.x - b, p.y - h), force, cachedCells);
        ExplodeRadius(p, sf::Vector2i(p.x + h, p.y + b), force, cachedCells);
        ExplodeRadius(p, sf::Vector2i(p.x - h, p.y + b), force, cachedCells);
        ExplodeRadius(p, sf::Vector2i(p.x + h, p.y - b), force, cachedCells);
        ExplodeRadius(p, sf::Vector2i(p.x - h, p.y - b), force, cachedCells);
    }
    room->QueueAction(room->ToIndex(p), Element::air);
    int ri = static_cast<int>(std::ceil(radius)) + 5;
    particles.BecomeParticle(p + sf::Vector2i {ri, ri}, 
        100.f * sf::Vector2f {1.f, 1.f}, 
        Element::fire, 
        properties.Colour(Element::fire));
    return true;
}

bool ActionWorker::ExplosionActOnOther(CellState &cell, ConstProperties &prop, sf::Vector2i p) {
    return false;
}
