#include "Constants.hpp"
#include "Interactions/ParticleWorker.hpp"
#include "Utility/Line.hpp"
#include "Utility/Physics.hpp"

ParticleWorker::ParticleWorker(roomID_t id, SandWorld &_world, SandRoom *_room, float _dt) :
    InteractionWorker(id, _world, _room, _dt), properties(_world.properties) {}

void ParticleWorker::BecomeParticle(sf::Vector2i p, sf::Vector2f F, Element id, sf::Color colour) {
    SandRoom *particleRoom = GetRoom(ContainingRoomID(p));

    // Remove the cell from the grid.
    size_t cellIndex = particleRoom->ToIndex(p);
    particleRoom->QueueAction(particleRoom->ToIndex(p), Element::air);

    // Add the particle to the system.
    Particle particle {id, p, colour};
    particleRoom->particles.AddParticle(particle, F);
}

void ParticleWorker::BecomeCell(size_t index) {
    // Convert the particle to a cell in the grid.
    sf::Vector2i p {room->particles[index].Position()};
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
        sf::Vector2i oldP {particle.Position()};
        particle.Integrate(dt);

        sf::Vector2i    dst;
        roomID_t        roomID;
        SandRoom       *dstRoom;
        bool collision = false;
        Lerp line {oldP, particle.Position()};
        for (Lerp::iterator lineIt = ++line.begin(); lineIt != line.end(); ++lineIt) {
            dst = *lineIt;
            roomID = ContainingRoomID(dst);
            if (!VALID_ROOM(roomID) && world.InBounds(dst)) {
                roomID = world.SpawnRoom(dst.x, dst.y);
            } else if (!VALID_ROOM(roomID)) {
                --lineIt;
                particle.Position(*lineIt);
                roomID = ContainingRoomID(dst);
                collision = true;
                break;
            }

            // Account for particles crossing rooms.
            dstRoom = GetRoom(roomID);
            if (roomID != thisID) {
                dstRoom->particles.AddParticle(particle);
                room->particles.RemoveParticle(i);
                i--;
                break;
            }

            if (!dstRoom->IsEmpty(dst)) {
                --lineIt;
                particle.Position(*lineIt);
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