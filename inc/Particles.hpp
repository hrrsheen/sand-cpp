#ifndef PARTICLES_HPP
#define PARTICLES_HPP

#include "Elements/Names.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>

struct Particle {
public:
    Element id = Element::null;
    sf::Color colour;

private:
    sf::Vector2f p  = {0.f, 0.f};
    sf::Vector2f v  = {0.f, 0.f};
    sf::Vector2f F  = {0.f, 0.f};

public:
    Particle(sf::Vector2i _p, Element _id, sf::Color _colour) : id(_id), p(_p), v(), F(), colour(_colour) {}

    // Returns the position of the particle (snapped to the grid).
    sf::Vector2i Position() const;
    // Sets the new position of the particle.
    void Position(sf::Vector2i newP);

    // Launch the particle with a given initial force.
    void ApplyForce(sf::Vector2f Fapplied={0.f, 0.f});
    void Integrate(float dt);
};

class ParticleSystem {
    std::vector<Particle> particles;
    size_t numParticles = 0;

public:
    // Adds a particle with the given properties to the system.
    void AddParticle(Particle &particle, sf::Vector2f Finit={0.f, 0.f});

    // Removes a particle at a given index from the system.
    void RemoveParticle(size_t index);

    // Returns the index range of the active particles.
    size_t Range() const;

    size_t Capacity() const;

    Particle& operator[](size_t n);
};

#endif