#include "Interactions/MovementWorker.hpp"

MovementWorker::MovementWorker(roomID_t id, SandWorld &_world, SandRoom *_room) : InteractionWorker(id, _world, _room) {}

bool MovementWorker::PerformMovement(CellState &cell, ConstProperties &prop, sf::Vector2i p) {
    if      (  MoveCell(cell, prop, p)) { return true; }  // Apply movement behaviours (falling, floating, etc).
    else if (SpreadCell(cell, prop, p)) { return true; }  // Apply spreading behaviour.

    return false;
}

void MovementWorker::SetDeltaTime(float _dt) {
    dt = _dt;
}

void MovementWorker::ConsolidateMovement() {
    if (room->queuedMoves.size() == 0) return;

    // Remove moves that have had their destination filled between frames.
    // TODO. Will probably be needed for threading?

    // Sort moves by source rooms then by destination.
    std::sort(room->queuedMoves.begin(), room->queuedMoves.end(), 
        [](const Move &a, const Move &b) { return a.dst < b.dst; }
    );
    
    // As the vector is sorted, this end object is guaranteed to be different to any other element.
    room->queuedMoves.emplace_back();

    int iStart {0};

    for (int i = 0; i < room->queuedMoves.size() - 1; ++i) {
        Move move       {room->queuedMoves.at(i)};
        Move nextMove   {room->queuedMoves.at(i + 1)};

        if (move.dst != nextMove.dst) {
            // Perform the randomly-selected move from the competing moves group.
            int iRand {iStart + QuickRandInt(i - iStart)};
            
            roomID_t id {room->queuedMoves[iRand].srcRoomID};
            int src     {room->queuedMoves[iRand].src};
            int dst     {room->queuedMoves[iRand].dst};
            
            SandRoom *srcRoom {GetRoom(id)};
            std::swap(srcRoom->grid.state[src], room->grid.state[dst]);
            std::swap(srcRoom->grid.colour[src], room->grid.colour[dst]);

            sf::Vector2i srcCoords {srcRoom->ToWorldCoords(src)};
            sf::Vector2i dstCoords {   room->ToWorldCoords(dst)};
            srcRoom->chunks.KeepContainingAlive(srcCoords.x, srcCoords.y);
               room->chunks.KeepContainingAlive(dstCoords.x, dstCoords.y);

            iStart = i + 1;
        }
    }

    room->queuedMoves.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
//  High-level behaviour.
//////////////////////////////////////////////////////////////////////////////////////////

bool MovementWorker::MoveCell(CellState &cell, ConstProperties &prop, sf::Vector2i p) {
    if (prop.moveBehaviour == MoveType::NONE) return false;

    if      (prop.moveBehaviour == MoveType::FLOAT_DOWN)  { return FloatDown   (p);     }
    else if (prop.moveBehaviour == MoveType::FLOAT_UP)    { return FloatUp     (p);     }
    else if (prop.moveBehaviour == MoveType::FALL_DOWN)   { return FallDown    (p, dt); }

    return false;
}

bool MovementWorker::SpreadCell(CellState &cell, ConstProperties &prop, sf::Vector2i p) {
    if (prop.spreadBehaviour == SpreadType::NONE) return false;
    cell.velocity = sf::Vector2f(0.f, 0.f); // Reset velocity. May later change to transferring y velocity to x.

    if      (prop.spreadBehaviour & SpreadType::DOWN_SIDE && SpreadDownSide(p)) { return true; }
    else if (prop.spreadBehaviour & SpreadType::UP_SIDE   && SpreadUpSide  (p)) { return true; } 
    else if (prop.spreadBehaviour & SpreadType::SIDE      && SpreadSide    (p)) { return true; }

    return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Specific movement behaviour.
//////////////////////////////////////////////////////////////////////////////////////////

bool MovementWorker::FloatDown(sf::Vector2i p) {
    sf::Vector2i queryPos(p.x, p.y - 1);
    // Handle the destination crossing rooms.
    if (room->IsEmpty(queryPos)) {
        room->QueueMovement(thisID, room->ToIndex(p.x, p.y), 
                                room->ToIndex(queryPos.x, queryPos.y));
        return true;
    }
    roomID_t id {world.EmptyRoom(queryPos)};
    if (VALID_ROOM(id)) {
        SandRoom &dstRoom {world.GetRoom(id)};
        dstRoom.QueueMovement(thisID, room->ToIndex(p.x, p.y),
                                dstRoom.ToIndex(queryPos.x, queryPos.y));
        return true;
    }

    return false;
}

bool MovementWorker::FloatUp(sf::Vector2i p) {
    return SpreadUpSide(p);
}

bool MovementWorker::FallDown(sf::Vector2i p, float dt) {
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
        dstRoom->QueueMovement(thisID, room->ToIndex(p.x, p.y), 
                                dstRoom->ToIndex(dst.x, dst.y));
        return true;
    }

    return false;
}

bool MovementWorker::SpreadDownSide(sf::Vector2i p) {
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
        dstRoom->QueueMovement(thisID, room->ToIndex(p), dstRoom->ToIndex( leftPos));
    } else if (VALID_ROOM(right)) {
        SandRoom *dstRoom {GetRoom(right)};
        dstRoom->QueueMovement(thisID, room->ToIndex(p), dstRoom->ToIndex(rightPos));
    }

    return VALID_ROOM(left) || VALID_ROOM(right);
}

bool MovementWorker::SpreadUpSide(sf::Vector2i p) {
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
        dstRoom->QueueMovement(thisID, room->ToIndex(p), dstRoom->ToIndex(leftPos));
    } else if (VALID_ROOM(right)) {
        dstRoom = GetRoom(right);
        dstRoom->QueueMovement(thisID, room->ToIndex(p), dstRoom->ToIndex(rightPos));
    }

    return VALID_ROOM(left) || VALID_ROOM(right);
}

bool MovementWorker::SpreadSide(sf::Vector2i p) {
    sf::Vector2i lookAhead  {1, 0};

    roomID_t left   {IsEmpty(p - lookAhead)}; 
    roomID_t right  {IsEmpty(p + lookAhead)};

    // Need to account for whether it's left OR right that gives a VALID_ROOM;
    if (VALID_ROOM(left) && VALID_ROOM(right)) {
        bool flip {Probability(50)};
        left    = BoolToID( left,  flip);
        right   = BoolToID(right, !flip);
    }

    int spreadRate {room->grid.SpreadRate(room->ToIndex(p))}; 
    
    sf::Vector2i dst;
    if (VALID_ROOM(left)) {
        std::tie( left, dst) = PathEmpty(p - lookAhead, p - spreadRate * lookAhead);
        SandRoom *dstRoom {GetRoom( left)};
        dstRoom->QueueMovement(thisID, room->ToIndex(p), dstRoom->ToIndex(dst));
    } else if (VALID_ROOM(right)) {
        std::tie(right, dst) = PathEmpty(p + lookAhead, p + spreadRate * lookAhead);
        SandRoom *dstRoom {GetRoom(right)};
        dstRoom->QueueMovement(thisID, room->ToIndex(p), dstRoom->ToIndex(dst));
    }

    return VALID_ROOM(left) || VALID_ROOM(right);
}