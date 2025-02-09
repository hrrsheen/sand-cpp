#include "Interactions/InteractionWorker.hpp"
#include "Utility/Line.hpp"

inline roomID_t BoolToID(roomID_t id, bool valid) {
    return (id * valid) + (valid - 1); // Should map (valid == true) -> id and (valid == false) -> -1.
}

InteractionWorker::InteractionWorker(roomID_t id, SandWorld &_world, SandRoom *_room, float _dt) :
    thisID(id), world(_world), room(_room), dt(_dt) {}

void InteractionWorker::KeepContainingAlive(int x, int y) {
    room->chunks.KeepContainingAlive(x, y);
    room->chunks.KeepNeighbourAlive(x, y);
}

void InteractionWorker::KeepNeighbourAlive(int x, int y) {
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

CellState& InteractionWorker::GetCell(int x, int y) {
    if (room->InBounds(x, y)) {
        return room->GetCell(x, y);
    }

    return world.GetCell(x, y);
}

CellState& InteractionWorker::GetCell(sf::Vector2i p) {
    return GetCell(p.x, p.y);
}

size_t InteractionWorker::CellIndex(sf::Vector2i p) {
    if (room->InBounds(p)) {
        return room->ToIndex(p);
    }

    return world.CellIndex(p);
}

void InteractionWorker::SetCell(int x, int y, Element id) {
    if (room->InBounds(x, y)) {
        return room->SetCell(x, y, id);
    }

    world.SetCell(x, y, id);
}

const ConstProperties& InteractionWorker::GetProperties(int index) const {
    return room->grid.GetProperties(index);
}

const ConstProperties& InteractionWorker::GetProperties(sf::Vector2i p) const {
    if (room->InBounds(p)) {
        return GetProperties(room->ToIndex(p));
    }

    return world.GetProperties(p);
}

roomID_t InteractionWorker::ContainingRoomID(sf::Vector2i p) {
    if (room->InBounds(p)) {
        return thisID;
    }

    return world.ContainingRoomID(p);
}

SandRoom* InteractionWorker::GetRoom(roomID_t id) {
    if (id == thisID) {
        return room;
    }

    return &world.GetRoom(id);
}

roomID_t InteractionWorker::IsEmpty(int x, int y) {
    return IsEmpty(sf::Vector2i(x, y));
}

roomID_t InteractionWorker::IsEmpty(sf::Vector2i p) {
    if (room->InBounds(p)) {
        bool empty {room->IsEmpty(p)};
        return BoolToID(thisID, empty); // Should map empty -> thisID and !empty -> -1.
    }

    return world.EmptyRoom(p);
}