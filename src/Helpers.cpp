#include "Helpers.hpp"
#include <random>
#include <SFML/System/Vector2.hpp>

namespace impl {

// Rounds a point to the nearest grid cell.
sf::Vector2i RoundPoint(sf::Vector2f p) {
    return sf::Vector2i(std::roundf(p.x), std::roundf(p.y));
}

// Returns the distance between two points.
int DiagonalDistance(sf::Vector2i p0, sf::Vector2i p1) {
    int dx {p1.x - p0.x};
    int dy {p1.y - p0.y};

    return std::max(std::abs(dx), std::abs(dy));
}

// Linearly interpolate between two numbers.
float lerp(float start, float end, float t) {
    return start * (1 - t) + t * end; // start + t * (end - start), just rearranged.
}

// Linearly interpolate between two points.
sf::Vector2f lerp(sf::Vector2f p0, sf::Vector2f p1, float t) {
    return sf::Vector2f(
        lerp(p0.x, p1.x, t),
        lerp(p0.y, p1.y, t)
    );
}

LerpIterator::LerpIterator(sf::Vector2f start, sf::Vector2f end, int step, int N) :
    start(start), end(end), step(step), N(N) {
}

// Dereference.
LerpIterator::value_type LerpIterator::operator*() const {
    float t = N == 0 ? 0.f : step / static_cast<float>(N);
    return RoundPoint(lerp(start, end, t));
}

// Pre-increment: ++it.
LerpIterator& LerpIterator::operator++() {
    ++step;
    return *this;
}

// Post-increment: it++.
LerpIterator LerpIterator::operator++(int) {
    LerpIterator tmp = *this;
    ++step;
    return tmp;
}

// Pre-decrement: --it.
LerpIterator& LerpIterator::operator--() {
    --step;
    return *this;
}

// Post-increment: it--.
LerpIterator LerpIterator::operator--(int) {
    LerpIterator tmp = *this;
    --step;
    return tmp;
}

// Equality: it == end().
bool LerpIterator::operator==(const LerpIterator &rhs) {
    return step == rhs.step && N == rhs.N;
}

bool LerpIterator::operator!=(const LerpIterator &rhs) {
    return !(*this == rhs);
}

}

Lerp::Lerp(sf::Vector2i start, sf::Vector2i finish) : 
    start(start), finish(finish), N(impl::DiagonalDistance(start, finish)) {
}

Lerp::iterator Lerp::begin() const {
    return impl::LerpIterator(start, finish, 0, N + 1);
}

Lerp::iterator Lerp::end() const {
    return impl::LerpIterator(start, finish, N + 2, N + 1);
}



std::mt19937 rng(std::random_device{}());

void InitRng() {
    std::srand(std::random_device{}());
    rng.discard(10000);
}

int QuickRandInt(int upper) {
    if (upper == 0) return 0;
    
    return std::rand() % upper;
}

int QuickRandRange(int a, int b) {
    return (std::rand() / static_cast<float>(RAND_MAX)) * (b - a) + a;
}

bool Probability(int percent) {
    if (percent <= 0) return false;
    
    return QuickRandInt(100) < percent - 1;
}

int RandInt(int upper) {
    return std::rand() % upper;
}

int RandRange(int a, int b) {
    return (std::rand() / static_cast<float>(RAND_MAX)) * (b - a) + a;
}
