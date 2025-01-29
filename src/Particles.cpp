#include "Constants.hpp"
#include "Particles.hpp"
#include "Utility/Physics.hpp"

void Particle::Integrate(float dt) {
    v = v + constants::accelGravity * dt;
    p = p + AccelerationDistance(v, dt);
}

void ParticleSystem::AddParticle(Element id, sf::Vector2i p, sf::Vector2f v, sf::Color colour) {
    if (numParticles < particles.size()) {
        particles[numParticles] = Particle {id, p, v, colour};
    } else {
        particles.emplace_back(id, p, v, colour);
    }
    numParticles++;
}

void ParticleSystem::AddParticle(Particle particle) {
    if (numParticles < particles.size()) {
        particles[numParticles] = particle;
    } else {
        particles.push_back(particle);
    }
    numParticles++;
}

void ParticleSystem::AddParticle(Particle &particle) {
    if (numParticles < particles.size()) {
        particles[numParticles] = particle;
    } else {
        particles.push_back(particle);
    }
    numParticles++;
}


void ParticleSystem::RemoveParticle(size_t index) {
    numParticles--;
    std::swap(particles[index], particles[numParticles]); // Move the particle that is to  deleted to the end of the active list.
}

size_t ParticleSystem::Range() const {
    return numParticles;
}

Particle& ParticleSystem::operator[](size_t n) {
    if (n > numParticles - 1) {
        throw std::out_of_range("Supplied index (n) is greater than the number of active particles.");
    }
    return particles.at(n);
}