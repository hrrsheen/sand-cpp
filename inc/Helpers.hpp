#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <iterator>
#include <optional>
#include <SFML/System/Vector2.hpp>

namespace impl {

class LerpIterator {
public:
    using value_type        = sf::Vector2i;
    using difference_type   = int;
    using pointer           = sf::Vector2i*;
    using reference         = sf::Vector2i&;
    using iterator_category = std::bidirectional_iterator_tag;

    explicit LerpIterator(sf::Vector2f start, sf::Vector2f end, int step, int N);

    // Dereference operator.
    value_type operator*() const;

    // Pre- and post-incrementable operators.
    LerpIterator& operator++();
    LerpIterator operator++(int);

    // Pre- and post-decrementable operators.
    LerpIterator& operator--();
    LerpIterator operator--(int);

    // Equality / inequality operators.
    bool operator==(const LerpIterator &rhs);
    bool operator!=(const LerpIterator &rhs);

private:
    const sf::Vector2f start;
    const sf::Vector2f end;
    const int N;
    int step;
};

}

class Lerp {
public:
    Lerp(sf::Vector2i start, sf::Vector2i finish);

    using iterator = impl::LerpIterator;
    iterator begin() const;
    iterator end() const;

private:
    const sf::Vector2f start;
    const sf::Vector2f finish;
    const int N;
};

namespace math {
    // Normalises the given vector to unit length.
    sf::Vector2f Normalise(sf::Vector2f v);

    // Rounds a point to the nearest grid cell.
    sf::Vector2i RoundPoint(sf::Vector2f p);
}

// Seeds the random number generator.
void InitRng();

// Returns a random integer from the interval [a, b).
// Uses std::rand(), which is messy but fast.
int QuickRandInt(int upper);

// Returns a random integer between a and b.
// Uses std::rand(), which is messy but fast.
int QuickRandRange(int a, int b);

// Returns true with a probability of the given percent.
bool Probability(int percent);

// Returns a random integer from the interval [a, b).
// Uses a Mersenne Twister, which is slow but provides better randomness.
int RandInt(int upper);

// Returns a random integer between a and b.
// Uses a Mersenne Twister, which is slow but provides better randomness.
int RandRange(int a, int b);

template <typename T, typename... Rest>
void HashCombine(std::size_t &seed, const T &v, const Rest&... rest) {
    seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    (HashCombine(seed, rest), ...);
}

#endif