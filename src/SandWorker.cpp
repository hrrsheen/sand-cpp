#include "Constants.hpp"
#include "SandWorker.hpp"

inline roomID_t BoolToID(roomID_t id, bool valid) {
    return (id * valid) + (valid - 1); // Should map (valid == true) -> id and (valid == false) -> -1.
}

SandWorker::SandWorker(roomID_t id, SandWorld &_world, SandRoom *_room) :
    thisID(id), world(_world), room(_room), properties(_world.properties), dt(0.f) {}


CellState& SandWorker::GetCell(int x, int y) {
    if (room->InBounds(x, y)) {
        return room->GetCell(x, y);
    }

    return world.GetCell(x, y);
}

CellState& SandWorker::GetCell(sf::Vector2i p) {
    return GetCell(p.x, p.y);
}

size_t SandWorker::CellIndex(sf::Vector2i p) {
    if (room->InBounds(p)) {
        return room->ToIndex(p);
    }

    return world.CellIndex(p);
}

void SandWorker::SetCell(int x, int y, Element id) {
    if (room->InBounds(x, y)) {
        return room->SetCell(x, y, id);
    }

    world.SetCell(x, y, id);
}

roomID_t SandWorker::ContainingRoomID(sf::Vector2i p) {
    if (room->InBounds(p)) {
        return thisID;
    }

    return world.ContainingRoomID(p);
}

SandRoom* SandWorker::GetRoom(roomID_t id) {
    if (id == thisID) {
        return room;
    }

    return &world.GetRoom(id);
}

roomID_t SandWorker::IsEmpty(int x, int y) {
    return IsEmpty(sf::Vector2i(x, y));
}

roomID_t SandWorker::IsEmpty(sf::Vector2i p) {
    if (room->InBounds(p)) {
        bool empty {room->IsEmpty(p)};
        return BoolToID(thisID, empty); // Should map empty -> thisID and !empty -> -1.
    }

    return world.EmptyRoom(p);
}

std::pair<roomID_t, sf::Vector2i> SandWorker::PathEmpty(sf::Vector2i start, sf::Vector2i end) {
    SandRoom *checkRoom {nullptr}, *validRoom {nullptr};
    roomID_t  checkID   {-1},       validID   {-1};

    sf::Vector2i dst {start};
    // Lerp lerp(start, end);
    for (sf::Vector2i check : Lerp {start, end}) {
        checkID = ContainingRoomID(check); // TODO: cache the currently checked room so that there's no need to re-get the ID / room.
        if (VALID_ROOM(checkID)) {
            checkRoom = GetRoom(checkID);            
        } else {
            break;
        }
        
        if (checkRoom->IsEmpty(check)) {
            dst = check;
            validRoom = checkRoom;
            validID = checkID;
        } else {
            break;
        }
    }

    return {validID, dst};
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Simulation.
//////////////////////////////////////////////////////////////////////////////////////////

void SandWorker::Step(float _dt) {
    // if (_dt > 1 / 60.f) _dt = 1 / 60.f; // DEBUG: Possibly remove this.
    dt = _dt;
    for (int ci = 0; ci < room->chunks.Size(); ++ci) {
        Chunk &chunk {room->chunks.GetChunk(ci)};
        SimulateChunk(chunk);

        room->chunks.UpdateChunk(ci);
    }

    room->ConsolidateActions();
    room->ConsolidateMoves(&world.rooms);
}

void SandWorker::SimulateChunk(Chunk &chunk) {
    for (int y = chunk.yMin; y < chunk.yMax; ++y) { // Inactive chunks will have yMin > yMax.
        // Process each row.
        // Alternate processing rows left to right and right to left.
        int dir {y % 2};
        int endpoints[] {chunk.xMin, chunk.xMax - 1};
        int start   {endpoints[1 - dir]};
        int end     {endpoints[dir]};
        dir = 2 * dir - 1;
        end += dir;
        for (int x = start; x != end; x += dir) {
            // Apply the simulation.
            if (ApplyRules(sf::Vector2i(x, y))) {
                room->chunks.KeepContainingAlive(x, y);
                room->chunks.KeepNeighbourAlive(x, y);
            }
        }
    }
}

bool SandWorker::ApplyRules(sf::Vector2i p) {
    CellState &cell {GetCell(p)};
    ConstProperties &prop {properties.constants[cell.id]};
    
    // if      (ActionCell(cell, prop, p)) { return true; }  // Act on other cells.
    if      (  MoveCell(cell, prop, p)) { return true; }
    // else if (  MoveCell(cell, prop, p)) { return true; }  // Apply movement behaviours (falling, floating, etc).
    else if (SpreadCell(cell, prop, p)) { return true; }  // Apply spreading behaviour.

    return false;
}


bool SandWorker::MoveCell(CellState &cell, ConstProperties &prop, sf::Vector2i p) {
    if (VALID_ROOM(IsEmpty(p.x, p.y))) return false;
    if (prop.moveBehaviour == MoveType::NONE) return false;

    if      (prop.moveBehaviour == MoveType::FLOAT_DOWN)  { return FloatDown   (p);     }
    else if (prop.moveBehaviour == MoveType::FLOAT_UP)    { return FloatUp     (p);     }
    else if (prop.moveBehaviour == MoveType::FALL_DOWN)   { return FallDown    (p, dt); }

    return false;
}

bool SandWorker::SpreadCell(CellState &cell, ConstProperties &prop, sf::Vector2i p) {
    if (VALID_ROOM(IsEmpty(p.x, p.y))) return false;
    if (prop.spreadBehaviour == SpreadType::NONE) return false;
    cell.velocity = sf::Vector2f(0.f, 0.f); // Reset velocity. May later change to transferring y velocity to x.

    if      (prop.spreadBehaviour & SpreadType::DOWN_SIDE && SpreadDownSide(p)) { return true; }
    else if (prop.spreadBehaviour & SpreadType::UP_SIDE   && SpreadUpSide  (p)) { return true; } 
    else if (prop.spreadBehaviour & SpreadType::SIDE      && SpreadSide    (p)) { return true; }

    return false;
}

// bool SandWorker::ActionCell(Cell &cell, ConstProperties &prop, sf::Vector2i p) {
//     if (VALID_ROOM(IsEmpty(p.x, p.y))) return false;

//     Action action {eleProp.ActUponSelf(p, cell, dt)};
//     if (action.IsValid()) {
//         room->QueueAction(action);
//         return true;
//     }

//     bool acted {false};
//     for (const sf::Vector2i &deltaP : eleProp.actionSet) {
//         roomID_t roomID {ContainingRoomID(p + deltaP)};
//         if (VALID_ROOM(roomID)) {
//             Cell &other {GetCell(p + deltaP)};
//             ElementProperties &otherProp {properties.Get(other.id)};
//             action = eleProp.ActUponOther(cell, eleProp, other, otherProp, p, p + deltaP, dt);
//             if (action.IsValid()) {
//                 acted = true;
//                 GetRoom(roomID)->QueueAction(action);
//             }
//         }
//     }

//     return acted;
// }


//////////////////////////////////////////////////////////////////////////////////////////
//  Movement.
//////////////////////////////////////////////////////////////////////////////////////////

bool SandWorker::FloatDown(sf::Vector2i p) {
    sf::Vector2i queryPos(p.x, p.y - 1);
    // Handle the destination crossing rooms.
    if (room->IsEmpty(queryPos)) {
        room->QueueMove(thisID, room->ToIndex(p.x, p.y), 
                                room->ToIndex(queryPos.x, queryPos.y));
        return true;
    }
    roomID_t id {world.EmptyRoom(queryPos)};
    if (VALID_ROOM(id)) {
        SandRoom &dstRoom {world.GetRoom(id)};
        dstRoom.QueueMove(thisID, room->ToIndex(p.x, p.y),
                                dstRoom.ToIndex(queryPos.x, queryPos.y));
        return true;
    }

    return false;
}

bool SandWorker::FloatUp(sf::Vector2i p) {
    return SpreadUpSide(p);
}

bool SandWorker::FallDown(sf::Vector2i p, float dt) {
    size_t iCell {CellIndex(p)};
    CellState &cell {room->GetCell(iCell)};
    cell.ApplyAcceleration(sf::Vector2f(0.f, -constants::accelGravity), dt);
    // If the resultant position a fractional part, use the fraction as a probability
    // to advance an additional cell.
    float yDst; // The y destination.
    int yRem {static_cast<int>(
        100.f * std::modf(cell.velocity.y * dt, &yDst) // The fractional part as a percentage.
    )};
    if (Probability(std::abs(yRem))) yDst -= 1.f; // The probability to advance an additional cell.
    sf::Vector2i deltaP {0, static_cast<int>(yDst)};

    auto [roomID, dst] = PathEmpty(p + sf::Vector2i(0, -1), p + deltaP);
    static_assert(std::is_same_v<decltype(roomID), roomID_t>);
    static_assert(std::is_same_v<decltype(dst), sf::Vector2i>);

    if (VALID_ROOM(roomID)) {
        SandRoom *dstRoom {GetRoom(roomID)};
        dstRoom->QueueMove(thisID, room->ToIndex(p.x, p.y), 
                                dstRoom->ToIndex(dst.x, dst.y));
        return true;
    }

    return false;
}

bool SandWorker::SpreadDownSide(sf::Vector2i p) {
    sf::Vector2i leftPos    {p + sf::Vector2i(-1, -1)};
    sf::Vector2i rightPos   {p + sf::Vector2i( 1, -1)};

    // These roomID_t will encode information about both cell openness and room validity. In the cases of either
    // an invalid room or a non-open cell, they will be -1. For an open cell in a valid room the variable will
    // simply hold the room's ID.
    roomID_t left   {ContainingRoomID( leftPos)}; 
    roomID_t right  {ContainingRoomID(rightPos)};

    CellState thisCell {GetCell(p)};
    // Determine whether the down-left cell is empty or displaceable.
    if (VALID_ROOM(left)) {
        CellState leftCell {GetCell(leftPos)};
        bool empty {GetRoom(left)->IsEmpty(p) || room->grid.CanDisplace(thisCell.id, leftCell.id)};
        left = BoolToID(left, empty);
    }
    // Determine whether the down-right cell is empty or displaceable.
    if (VALID_ROOM(right)) {
        CellState rightCell {GetCell(rightPos)};
        bool empty {GetRoom(right)->IsEmpty(p) || room->grid.CanDisplace(thisCell.id, rightCell.id)};
        right = BoolToID(right, empty);
    }

    // If both left and right are open / displaceable spaces, randomly select one.
    if (VALID_ROOM(left) && VALID_ROOM(right)) {
        bool flip {Probability(50)};
        left    = BoolToID( left,  flip);
        right   = BoolToID(right, !flip);
    }

    if (VALID_ROOM(left)) {
        SandRoom *dstRoom {GetRoom( left)};
        dstRoom->QueueMove(thisID, room->ToIndex(p), dstRoom->ToIndex( leftPos));
    } else if (VALID_ROOM(right)) {
        SandRoom *dstRoom {GetRoom(right)};
        dstRoom->QueueMove(thisID, room->ToIndex(p), dstRoom->ToIndex(rightPos));
    }

    return VALID_ROOM(left) || VALID_ROOM(right);
}

bool SandWorker::SpreadUpSide(sf::Vector2i p) {
    sf::Vector2i leftPos    {p + sf::Vector2i( 1, 1)};
    sf::Vector2i rightPos   {p + sf::Vector2i(-1, 1)};

    roomID_t left   {IsEmpty( leftPos)}; 
    roomID_t right  {IsEmpty(rightPos)};

    // If both left and right are open spaces, randomly select one.
    if (VALID_ROOM(left) && VALID_ROOM(right)) {
        bool flip {Probability(50)};
        left    = BoolToID( left,  flip);
        right   = BoolToID(right, !flip);
    }

    SandRoom *dstRoom;
    if (VALID_ROOM(left)) {
        dstRoom = GetRoom(left);
        dstRoom->QueueMove(thisID, room->ToIndex(p), dstRoom->ToIndex(leftPos));
    } else if (VALID_ROOM(right)) {
        dstRoom = GetRoom(right);
        dstRoom->QueueMove(thisID, room->ToIndex(p), dstRoom->ToIndex(rightPos));
    }

    return VALID_ROOM(left) || VALID_ROOM(right);
}

bool SandWorker::SpreadSide(sf::Vector2i p) {
    sf::Vector2i lookAhead  {1, 0};

    roomID_t left   {IsEmpty(p - lookAhead)}; 
    roomID_t right  {IsEmpty(p + lookAhead)};

    // Need to account for whether it's left OR right that gives a VALID_ROOM;
    if (VALID_ROOM(left) && VALID_ROOM(right)) {
        bool flip {Probability(50)};
        left    = BoolToID( left,  flip);
        right   = BoolToID(right, !flip);
    }

    int spreadRate {GetRoom(ContainingRoomID(p))->grid.SpreadRate(room->ToIndex(p))}; // TODO: This is gross. There has to be a better way.
    
    sf::Vector2i dst;
    if (VALID_ROOM(left)) {
        std::tie( left, dst) = PathEmpty(p - lookAhead, p - spreadRate * lookAhead);
        SandRoom *dstRoom {GetRoom( left)};
        dstRoom->QueueMove(thisID, room->ToIndex(p), dstRoom->ToIndex(dst));
    } else if (VALID_ROOM(right)) {
        std::tie(right, dst) = PathEmpty(p + lookAhead, p + spreadRate * lookAhead);
        SandRoom *dstRoom {GetRoom(right)};
        dstRoom->QueueMove(thisID, room->ToIndex(p), dstRoom->ToIndex(dst));
    }

    return VALID_ROOM(left) || VALID_ROOM(right);
}