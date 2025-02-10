#include "Constants.hpp"
#include "SandWorker.hpp"
#include "Utility/Physics.hpp"
#include "Utility/Random.hpp"

SandWorker::SandWorker(roomID_t id, SandWorld &world, SandRoom *room, float _dt) :
    del(id, world, room), dt(_dt) {}

//////////////////////////////////////////////////////////////////////////////////////////
//  Simulation.
//////////////////////////////////////////////////////////////////////////////////////////

void SandWorker::UpdateRoom() {
    ProcessParticles();
    for (int ci = 0; ci < del.room->chunks.Size(); ++ci) {
        Chunk &chunk {del.room->chunks.GetChunk(ci)};
        del.room->chunks.UpdateChunk(ci);

        SimulateChunk(chunk);
    }
}

void SandWorker::SimulateChunk(Chunk &chunk) {
    for (int y = chunk.yMin; y < chunk.yMax; ++y) { // Inactive chunks will have yMin > yMax.
        // Process each row.
        // Alternate processing rows left to right and right to left.
        int dir = std::abs(y % 2);
        int endpoints[] {chunk.xMin, chunk.xMax - 1};
        int start   = endpoints[1 - dir];
        int end     = endpoints[dir];
        dir = 2 * dir - 1;
        end += dir;
        for (int x = start; x != end; x += dir) {
            // Apply the simulation.
            if (SimulateCell(sf::Vector2i(x, y))) {
                del.KeepContainingAlive(x, y);
                del.KeepNeighbourAlive(x, y);
            }
        }
    }
}

bool SandWorker::SimulateCell(sf::Vector2i p) {
    if (del.room->IsEmpty(p.x, p.y)) return false;

    CellState &cell {del.room->GetCell(p)};
    ConstProperties &prop {del.properties.constants[cell.id]};
    
    if      ( PerformActions(p, cell, prop)) { return true; }  // Act on other cells.
    else if (PerformMovement(p, cell, prop)) { return true; }  // Move the cell.

    return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Cell movement functions
//////////////////////////////////////////////////////////////////////////////////////////////////

bool SandWorker::PerformMovement(sf::Vector2i p, CellState &cell, ConstProperties &prop) {
    if      (  MoveCell(p, cell, prop)) { return true; }  // Apply movement behaviours (falling, floating, etc).
    else if (SpreadCell(p, cell, prop)) { return true; }  // Apply spreading behaviour.

    return false;
}

bool SandWorker::MoveCell(sf::Vector2i p, CellState &cell, ConstProperties &prop) {
    if (prop.moveBehaviour == MoveType::NONE) return false;
    if (cell.velocity == sf::Vector2f(0.f, 0.f)) cell.velocity = constants::initialV;

    if      (prop.moveBehaviour == MoveType::FLOAT_DOWN)  { return FloatDown   (p); }
    else if (prop.moveBehaviour == MoveType::FLOAT_UP)    { return FloatUp     (p); }
    else if (prop.moveBehaviour == MoveType::FALL_DOWN)   { return FallDown    (p); }

    return false;
}

bool SandWorker::SpreadCell(sf::Vector2i p, CellState &cell, ConstProperties &prop) {
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

bool SandWorker::FloatDown(sf::Vector2i p) {
    sf::Vector2i queryPos(p.x, p.y - 1);
    // Handle the destination crossing rooms.
    if (del.room->IsEmpty(queryPos)) {
        del.room->QueueMovement(del.thisID, del.room->ToIndex(p.x, p.y), 
                                            del.room->ToIndex(queryPos.x, queryPos.y));
        return true;
    }
    roomID_t id {del.world.EmptyRoom(queryPos)};
    if (VALID_ROOM(id)) {
        SandRoom &dstRoom {del.world.GetRoom(id)};
        dstRoom.QueueMovement(del.thisID, del.room->ToIndex(p.x, p.y),
                                            dstRoom.ToIndex(queryPos.x, queryPos.y));
        return true;
    }

    return false;
}

bool SandWorker::FloatUp(sf::Vector2i p) {
    return SpreadUpSide(p);
}

bool SandWorker::FallDown(sf::Vector2i p) {
    size_t iCell    = del.CellIndex(p);
    CellState &cell = del.room->GetCell(iCell);

    cell.ApplyAcceleration(constants::accelGravity, dt);
    sf::Vector2i deltaP {AccelerateProbability(cell.velocity, dt)};

    roomID_t roomID;
    sf::Vector2i dst;
    std::tie(roomID, dst) = del.PathEmpty<PathOpts::SPAWN>(p + sf::Vector2i {0, -1}, p + deltaP);

    if (VALID_ROOM(roomID)) {
        SandRoom *dstRoom = del.GetRoom(roomID);
        dstRoom->QueueMovement(del.thisID, del.room->ToIndex(p.x, p.y), 
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
    roomID_t left   = del.ContainingRoomID( leftPos);
    roomID_t right  = del.ContainingRoomID(rightPos);

    CellState thisCell {del.GetCell(p)};
    // Determine whether the down-left cell is empty or displaceable.
    if (VALID_ROOM(left)) {
        CellState leftCell {del.GetCell(leftPos)};
        bool empty = del.GetRoom(left)->IsEmpty(p) || del.room->grid.CanDisplace(thisCell.id, leftCell.id);
        left = BoolToID(left, empty);
    }
    // Determine whether the down-right cell is empty or displaceable.
    if (VALID_ROOM(right)) {
        CellState rightCell {del.GetCell(rightPos)};
        bool empty = del.GetRoom(right)->IsEmpty(p) || del.room->grid.CanDisplace(thisCell.id, rightCell.id);
        right = BoolToID(right, empty);
    }

    // If both left and right are open / displaceable spaces, randomly select one.
    if (VALID_ROOM(left) && VALID_ROOM(right)) {
        bool flip = Probability(50);
        left    = BoolToID( left,  flip);
        right   = BoolToID(right, !flip);
    }

    if (VALID_ROOM(left)) {
        SandRoom *dstRoom = del.GetRoom( left);
        dstRoom->QueueMovement(del.thisID, del.room->ToIndex(p), dstRoom->ToIndex( leftPos));
    } else if (VALID_ROOM(right)) {
        SandRoom *dstRoom = del.GetRoom(right);
        dstRoom->QueueMovement(del.thisID, del.room->ToIndex(p), dstRoom->ToIndex(rightPos));
    }

    return VALID_ROOM(left) || VALID_ROOM(right);
}

bool SandWorker::SpreadUpSide(sf::Vector2i p) {
    sf::Vector2i leftPos    {p + sf::Vector2i( 1, 1)};
    sf::Vector2i rightPos   {p + sf::Vector2i(-1, 1)};

    roomID_t left   = del.IsEmpty( leftPos);
    roomID_t right  = del.IsEmpty(rightPos);

    // If both left and right are open spaces, randomly select one.
    if (VALID_ROOM(left) && VALID_ROOM(right)) {
        bool flip = Probability(50);
        left    = BoolToID( left,  flip);
        right   = BoolToID(right, !flip);
    }

    if (VALID_ROOM(left)) {
        SandRoom *dstRoom = del.GetRoom(left);
        dstRoom->QueueMovement(del.thisID, del.room->ToIndex(p), dstRoom->ToIndex(leftPos));
    } else if (VALID_ROOM(right)) {
        SandRoom *dstRoom = del.GetRoom(right);
        dstRoom->QueueMovement(del.thisID, del.room->ToIndex(p), dstRoom->ToIndex(rightPos));
    }

    return VALID_ROOM(left) || VALID_ROOM(right);
}

bool SandWorker::SpreadSide(sf::Vector2i p) {
    sf::Vector2i lookAhead {1, 0};
    int spreadRate = del.room->grid.SpreadRate(del.room->ToIndex(p));

    auto [ left,  leftDst] = del.PathEmpty<PathOpts::SKIP | PathOpts::SPAWN>(p, p - spreadRate * lookAhead);
    auto [right, rightDst] = del.PathEmpty<PathOpts::SKIP | PathOpts::SPAWN>(p, p + spreadRate * lookAhead);

    // Need to account for whether it's left OR right that gives a VALID_ROOM;
    if (VALID_ROOM(left) && VALID_ROOM(right)) {
        bool flip {Probability(50)};
        left    = BoolToID( left,  flip);
        right   = BoolToID(right, !flip);
    }
    
    if (VALID_ROOM(left)) {
        SandRoom *dstRoom = del.GetRoom( left);
        dstRoom->QueueMovement(del.thisID, del.room->ToIndex(p), dstRoom->ToIndex(leftDst));
    } else if (VALID_ROOM(right)) {
        SandRoom *dstRoom = del.GetRoom(right);
        dstRoom->QueueMovement(del.thisID, del.room->ToIndex(p), dstRoom->ToIndex(rightDst));
    }

    return VALID_ROOM(left) || VALID_ROOM(right);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Cell movement functions
//////////////////////////////////////////////////////////////////////////////////////////////////

bool SandWorker::PerformActions(sf::Vector2i p, CellState &cell, ConstProperties &prop) {
    // if      (ActOnSelf (p, cell, prop)) { return true; } 
    // else if (ActOnOther(p, cell, prop)) { return true; }

    return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Particle simulation
//////////////////////////////////////////////////////////////////////////////////////////////////

void SandWorker::ProcessParticles() {
    for (int i = 0; i < del.room->particles.Range(); i++) {
        // Calcualte the particle's next position (before any collisions).
        Particle &particle {del.room->particles[i]};
        sf::Vector2i oldP {particle.Position()};
        particle.Integrate(dt);

        sf::Vector2i    dst;
        roomID_t        roomID;
        SandRoom       *dstRoom;
        bool collision = false;

        Lerp line {oldP, particle.Position()};
        // Check the path to the new position for collisions.
        for (Lerp::iterator lineIt = ++line.begin(); lineIt != line.end(); ++lineIt) {
            dst = *lineIt;

            // Account for particles encountering the world edge.
            roomID = del.ContainingRoomID(dst);
            // Handle the case where the world boundary has not been reached, but the room is not spawned.
            if (!VALID_ROOM(roomID) && del.world.InBounds(dst)) {
                roomID = del.world.SpawnRoom(dst.x, dst.y);
            // Handle the case where the would boundary is reaced.
            } else if (!VALID_ROOM(roomID)) {
                --lineIt;
                particle.Position(*lineIt);
                roomID = del.ContainingRoomID(dst);
                collision = true;
                break;
            }

            // Account for particles crossing rooms.
            dstRoom = del.GetRoom(roomID);
            if (roomID != del.thisID) {
                dstRoom->particles.AddParticle(particle);
                del.room->particles.RemoveParticle(i);
                i--;
                break;
            }

            // The particle has collided with a cell.
            if (!dstRoom->IsEmpty(dst)) {
                --lineIt;
                particle.Position(*lineIt);
                roomID = del.ContainingRoomID(dst);
                collision = true;
                break;
            }
        }

        // The path to the particle's destination contains a collision, 
        // therefore convert the particle to a cell.
        if (collision) {
            ParticleToCell(i);
            // Deactivating a particle will move a different particle into its index. Decrement the index
            // here so that next iteration of the loop we continue from the same index.
            i--;
        }
    }
}

void SandWorker::ParticleToCell(size_t index) {
    // Convert the particle to a cell in the grid.
    sf::Vector2i p {del.room->particles[index].Position()};
    del.room->grid.Assign(
        del.room->ToIndex(p), 
        del.room->particles[index].id,
        del.room->particles[index].colour);
    // Remove the particle from the system.
    del.room->particles.RemoveParticle(index);
    del.KeepContainingAlive(p.x, p.y);
}