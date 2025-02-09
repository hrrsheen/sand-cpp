#include "Interactions/MovementWorker.hpp"
#include "Utility/Physics.hpp"
#include "Utility/Random.hpp"

MovementWorker::MovementWorker(roomID_t id, SandWorld &_world, SandRoom *_room, float _dt) : InteractionWorker(id, _world, _room, _dt) {}

bool MovementWorker::PerformMovement(sf::Vector2i p, CellState &cell, ConstProperties &prop) {
    if      (  MoveCell(p, cell, prop)) { return true; }  // Apply movement behaviours (falling, floating, etc).
    else if (SpreadCell(p, cell, prop)) { return true; }  // Apply spreading behaviour.

    return false;
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

bool MovementWorker::MoveCell(sf::Vector2i p, CellState &cell, ConstProperties &prop) {
    if (prop.moveBehaviour == MoveType::NONE) return false;
    if (cell.velocity == sf::Vector2f(0.f, 0.f)) cell.velocity = constants::initialV;

    if      (prop.moveBehaviour == MoveType::FLOAT_DOWN)  { return FloatDown   (p); }
    else if (prop.moveBehaviour == MoveType::FLOAT_UP)    { return FloatUp     (p); }
    else if (prop.moveBehaviour == MoveType::FALL_DOWN)   { return FallDown    (p); }

    return false;
}

bool MovementWorker::SpreadCell(sf::Vector2i p, CellState &cell, ConstProperties &prop) {
    if (prop.spreadBehaviour == SpreadType::NONE) return false;

    if      (prop.spreadBehaviour & SpreadType::DOWN_SIDE && SpreadDownSide(p)) { return true; }
    else if (prop.spreadBehaviour & SpreadType::UP_SIDE   && SpreadUpSide  (p)) { return true; } 
    else if (prop.spreadBehaviour & SpreadType::SIDE      && SpreadSide    (p)) { return true; }

    cell.velocity = sf::Vector2f(0.f, 0.f); // Reset velocity. May later change to transferring y velocity to x.
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

bool MovementWorker::FallDown(sf::Vector2i p) {
    size_t iCell    = CellIndex(p);
    CellState &cell = room->GetCell(iCell);

    cell.ApplyAcceleration(constants::accelGravity, dt);
    sf::Vector2i deltaP {AccelerateProbability(cell.velocity, dt)};

    roomID_t roomID;
    sf::Vector2i dst;
    std::tie(roomID, dst) = PathEmpty<PathOpts::SPAWN>(p + sf::Vector2i {0, -1}, p + deltaP);

    if (VALID_ROOM(roomID)) {
        SandRoom *dstRoom = GetRoom(roomID);
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
    roomID_t left   = ContainingRoomID( leftPos);
    roomID_t right  = ContainingRoomID(rightPos);

    CellState thisCell {GetCell(p)};
    // Determine whether the down-left cell is empty or displaceable.
    if (VALID_ROOM(left)) {
        CellState leftCell {GetCell(leftPos)};
        bool empty = GetRoom(left)->IsEmpty(p) || room->grid.CanDisplace(thisCell.id, leftCell.id);
        left = BoolToID(left, empty);
    }
    // Determine whether the down-right cell is empty or displaceable.
    if (VALID_ROOM(right)) {
        CellState rightCell {GetCell(rightPos)};
        bool empty = GetRoom(right)->IsEmpty(p) || room->grid.CanDisplace(thisCell.id, rightCell.id);
        right = BoolToID(right, empty);
    }

    // If both left and right are open / displaceable spaces, randomly select one.
    if (VALID_ROOM(left) && VALID_ROOM(right)) {
        bool flip = Probability(50);
        left    = BoolToID( left,  flip);
        right   = BoolToID(right, !flip);
    }

    if (VALID_ROOM(left)) {
        SandRoom *dstRoom = GetRoom( left);
        dstRoom->QueueMovement(thisID, room->ToIndex(p), dstRoom->ToIndex( leftPos));
    } else if (VALID_ROOM(right)) {
        SandRoom *dstRoom = GetRoom(right);
        dstRoom->QueueMovement(thisID, room->ToIndex(p), dstRoom->ToIndex(rightPos));
    }

    return VALID_ROOM(left) || VALID_ROOM(right);
}

bool MovementWorker::SpreadUpSide(sf::Vector2i p) {
    sf::Vector2i leftPos    {p + sf::Vector2i( 1, 1)};
    sf::Vector2i rightPos   {p + sf::Vector2i(-1, 1)};

    roomID_t left   = IsEmpty( leftPos);
    roomID_t right  = IsEmpty(rightPos);

    // If both left and right are open spaces, randomly select one.
    if (VALID_ROOM(left) && VALID_ROOM(right)) {
        bool flip = Probability(50);
        left    = BoolToID( left,  flip);
        right   = BoolToID(right, !flip);
    }

    if (VALID_ROOM(left)) {
        SandRoom *dstRoom = GetRoom(left);
        dstRoom->QueueMovement(thisID, room->ToIndex(p), dstRoom->ToIndex(leftPos));
    } else if (VALID_ROOM(right)) {
        SandRoom *dstRoom = GetRoom(right);
        dstRoom->QueueMovement(thisID, room->ToIndex(p), dstRoom->ToIndex(rightPos));
    }

    return VALID_ROOM(left) || VALID_ROOM(right);
}

bool MovementWorker::SpreadSide(sf::Vector2i p) {
    sf::Vector2i lookAhead {1, 0};
    int spreadRate = room->grid.SpreadRate(room->ToIndex(p));

    auto [ left,  leftDst] = PathEmpty<PathOpts::SKIP | PathOpts::SPAWN>(p, p - spreadRate * lookAhead);
    auto [right, rightDst] = PathEmpty<PathOpts::SKIP | PathOpts::SPAWN>(p, p + spreadRate * lookAhead);

    // Need to account for whether it's left OR right that gives a VALID_ROOM;
    if (VALID_ROOM(left) && VALID_ROOM(right)) {
        bool flip {Probability(50)};
        left    = BoolToID( left,  flip);
        right   = BoolToID(right, !flip);
    }
    
    if (VALID_ROOM(left)) {
        SandRoom *dstRoom = GetRoom( left);
        dstRoom->QueueMovement(thisID, room->ToIndex(p), dstRoom->ToIndex(leftDst));
    } else if (VALID_ROOM(right)) {
        SandRoom *dstRoom = GetRoom(right);
        dstRoom->QueueMovement(thisID, room->ToIndex(p), dstRoom->ToIndex(rightDst));
    }

    return VALID_ROOM(left) || VALID_ROOM(right);
}
