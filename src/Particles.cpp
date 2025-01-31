#include "Constants.hpp"
#include "Particles.hpp"
#include "Utility/Physics.hpp"

sf::Vector2i Particle::Position() const {
    return sf::Vector2i {
        static_cast<int>(std::roundf(p.x)),
        static_cast<int>(std::roundf(p.y))
    };
}

void Particle::Position(sf::Vector2i newP) {
    p = sf::Vector2f {newP};
}

void Particle::ApplyForce(sf::Vector2f Fapplied) {
    F = Fapplied;
}

void Particle::Integrate(float dt) {
    // Calculate the drag force.
    float vMag = std::sqrtf(v.x * v.x + v.y * v.y);

    sf::Vector2f Fdrag {-constants::k * vMag * v};

    sf::Vector2f a {((F + Fdrag) / constants::M) + 5.f * constants::accelGravity};
    v = v + a * dt;
    p = p + v * dt;

    ApplyForce();
}

void ParticleSystem::AddParticle(Particle &particle, sf::Vector2f Finit) {
    if (numParticles < particles.size()) {
        particles[numParticles] = particle;
    } else {
        particles.push_back(particle);
    }
    particles[numParticles].ApplyForce(Finit);

    numParticles++;
}


void ParticleSystem::RemoveParticle(size_t index) {
    numParticles--;
    // std::swap(particles[index], particles[numParticles]); // Move the particle that is to  deleted to the end of the active list.
    Particle tmp {particles[numParticles]};
    particles[index] = tmp;

    // return particles[numParticles];
}

size_t ParticleSystem::Range() const {
    return numParticles;
}

size_t ParticleSystem::Capacity() const {
    return particles.size();
}

Particle& ParticleSystem::operator[](size_t n) {
    if (n > numParticles - 1) {
        throw std::out_of_range("Supplied index (n) is greater than the number of active particles.");
    }
    return particles.at(n);
}