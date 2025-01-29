#ifndef INTERACTIONS_PARTICLE_WORKER_HPP
#define INTERACTIONS_PARTICLE_WORKER_HPP

#include "Elements/Names.hpp"
#include "Interactions/InteractionWorker.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

class ParticleWorker : public InteractionWorker {
private:
    ElementProperties &properties;

public:
    ParticleWorker(roomID_t id, SandWorld &_world, SandRoom *_room);

    void BecomeParticle(sf::Vector2i p, sf::Vector2f v, Element id, sf::Color colour);
    void BecomeCell(size_t index);

    void ProcessParticles();
};

#endif