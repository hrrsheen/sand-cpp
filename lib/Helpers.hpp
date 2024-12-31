#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <iterator>
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

// Seeds the random number generator.
void InitRng();

// Returns a random integer from the interval [a, b).
// Uses std::rand(), which is messy but fast.
int QuickRandInt(int upper);

// Returns a random integer between a and b.
// Uses std::rand(), which is messy but fast.
int QuickRandRange(int a, int b);

// Returns a random integer from the interval [a, b).
// Uses a Mersenne Twister, which is slow but provides better randomness.
int RandInt(int upper);

// Returns a random integer between a and b.
// Uses a Mersenne Twister, which is slow but provides better randomness.
int RandRange(int a, int b);

#endif