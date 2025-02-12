#include "WorldDelegate.hpp"
#include "Utility/Line.hpp"

inline roomID_t BoolToID(roomID_t id, bool valid) {
    return (id * valid) + (valid - 1); // Should map (valid == true) -> id and (valid == false) -> -1.
}

WorldDelegate::WorldDelegate(roomID_t id, SandWorld &_world, SandRoom *_room, ElementProperties &_properties) :
    thisID(id), world(_world), room(_room), properties(_properties) {}

void WorldDelegate::SetDeltaTime(float _dt) { dt = _dt; }

////////////////////////////////////////////////////////////////////////////////////////
// Chunk manipulation
////////////////////////////////////////////////////////////////////////////////////////

void WorldDelegate::KeepContainingAlive(int x, int y) {
    room->chunks.KeepContainingAlive(x, y);
    room->chunks.KeepNeighbourAlive(x, y);
}

void WorldDelegate::KeepNeighbourAlive(int x, int y) {
    if (x == room->x)                   
        x -= 1;
    else if (x == room->x + room->width - 1) 
        x += 1;

    if (y == room->y)
        y -= 1;
    else if (y == room->y + room->height - 1) 
        y += 1;

    roomID_t id {ContainingRoomID(sf::Vector2i{x, y})};
    if (id != thisID && VALID_ROOM(id)) {
        world.GetRoom(id).chunks.KeepContainingAlive(x, y);
    }
}

////////////////////////////////////////////////////////////////////////////////////////
// Cell manipulation
////////////////////////////////////////////////////////////////////////////////////////

CellState& WorldDelegate::GetCell(int x, int y) {
    if (room->InBounds(x, y)) {
        return room->GetCell(x, y);
    }

    return world.GetCell(x, y);
}

CellState& WorldDelegate::GetCell(sf::Vector2i p) {
    return GetCell(p.x, p.y);
}

size_t WorldDelegate::CellIndex(sf::Vector2i p) {
    if (room->InBounds(p)) {
        return room->ToIndex(p);
    }

    return world.CellIndex(p);
}

////////////////////////////////////////////////////////////////////////////////////////
// Particle manipulation
////////////////////////////////////////////////////////////////////////////////////////

void WorldDelegate::CellToParticle(sf::Vector2i p, sf::Vector2f F) {
    SandRoom *particleRoom = GetRoom(ContainingRoomID(p));

    // Remove the cell from the grid.
    size_t cellIndex = particleRoom->ToIndex(p);
    particleRoom->QueueAction(particleRoom->ToIndex(p), Element::air);

    // Add the particle to the system.
    Element id = particleRoom->GetCell(p).id;
    Particle particle {p, 
        particleRoom->grid.state[cellIndex].id,
        particleRoom->grid.colour[cellIndex]}; // TODO: Account for velocity?
    particleRoom->particles.AddParticle(particle, F);
}

void WorldDelegate::SpawnParticle(sf::Vector2i p, sf::Vector2f F, Element id, sf::Color colour) {
    SandRoom *particleRoom = GetRoom(ContainingRoomID(p));

    // Remove the cell from the grid.
    size_t cellIndex = particleRoom->ToIndex(p);
    particleRoom->QueueAction(particleRoom->ToIndex(p), Element::air);

    // Add the particle to the system.
    Particle particle {p, id, colour};
    particleRoom->particles.AddParticle(particle, F);
}


////////////////////////////////////////////////////////////////////////////////////////
// Element queries
////////////////////////////////////////////////////////////////////////////////////////

const ConstProperties& WorldDelegate::GetProperties(Element id) const {
    return properties.constants[id];
}

const ConstProperties& WorldDelegate::GetProperties(int index) const {
    return GetProperties(room->grid.state[index].id);
}

const ConstProperties& WorldDelegate::GetProperties(sf::Vector2i p) const {
    if (room->InBounds(p)) {
        return GetProperties(room->ToIndex(p));
    }

    SandRoom &containingRoom {world.GetContainingRoom(p.x, p.y)};
    return GetProperties(containingRoom.ToIndex(p));
}

////////////////////////////////////////////////////////////////////////////////////////
// Room queries
////////////////////////////////////////////////////////////////////////////////////////

roomID_t WorldDelegate::ContainingRoomID(sf::Vector2i p) {
    if (room->InBounds(p)) {
        return thisID;
    }

    return world.ContainingRoomID(p);
}

SandRoom* WorldDelegate::GetRoom(roomID_t id) {
    if (id == thisID) {
        return room;
    }

    return &world.GetRoom(id);
}

roomID_t WorldDelegate::IsEmpty(int x, int y) {
    return IsEmpty(sf::Vector2i(x, y));
}

roomID_t WorldDelegate::IsEmpty(sf::Vector2i p) {
    if (room->InBounds(p)) {
        bool empty {room->IsEmpty(p)};
        return BoolToID(thisID, empty); // Should map empty -> thisID and !empty -> -1.
    }

    return world.EmptyRoom(p);
}