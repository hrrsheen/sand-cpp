#include "Constants.hpp"
#include "Interactions/ParticleWorker.hpp"
#include "Utility/Line.hpp"
#include "Utility/Physics.hpp"

ParticleWorker::ParticleWorker(roomID_t id, SandWorld &_world, SandRoom *_room) :
    InteractionWorker(id, _world, _room), properties(_world.properties) {}

void ParticleWorker::BecomeParticle(sf::Vector2i p, sf::Vector2f v, Element id, sf::Color colour) {
    SandRoom *particleRoom = GetRoom(ContainingRoomID(p));

    // Remove the cell from the grid.
    size_t cellIndex = particleRoom->ToIndex(p);
    particleRoom->QueueAction(particleRoom->ToIndex(p), Element::air);

    // Add the particle to the system.
    particleRoom->particles.AddParticle(id, p, v, colour);
}

void ParticleWorker::BecomeCell(size_t index) {
    // Convert the particle to a cell in the grid.
    sf::Vector2i p {room->particles[index].p};
    room->grid.Assign(
        room->ToIndex(p), 
        room->particles[index].id,
        room->particles[index].colour);
    // Remove the particle from the system.
    room->particles.RemoveParticle(index);
    KeepContainingAlive(p.x, p.y);
}

void ParticleWorker::ProcessParticles() {
    for (int i = 0; i < room->particles.Range(); i++) {
        Particle &particle {room->particles[i]};
        sf::Vector2i oldP {particle.p};
        particle.Integrate(dt);

        sf::Vector2i    dst;
        roomID_t        roomID;
        SandRoom       *dstRoom;
        bool collision = false;
        Lerp line {oldP, particle.p};
        for (Lerp::iterator lineIt = ++line.begin(); lineIt != line.end(); ++lineIt) {
            dst = *lineIt;
            roomID = ContainingRoomID(dst);
            if (!VALID_ROOM(roomID) && world.InBounds(dst)) {
                roomID = world.SpawnRoom(dst.x, dst.y);
            } else if (!VALID_ROOM(roomID)) {
                --lineIt;
                particle.p = *lineIt;
                roomID = ContainingRoomID(dst);
                collision = true;
                break;
            }

            dstRoom = GetRoom(roomID);
            if (!dstRoom->IsEmpty(dst)) {
                --lineIt;
                particle.p = *lineIt;
                roomID = ContainingRoomID(dst);
                collision = true;
                break;
            }
        }

        // The path to the particle's destination contains a collision, 
        // therefore convert the particle to a cell.
        if (collision) {
            BecomeCell(i);
            i--;
        }
    }
}