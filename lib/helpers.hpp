#ifndef HELPERS_HPP
#define HELPERS_HPP

// Seeds the random number generator.
void initRng();

// Returns a random integer from the interval [a, b).
// Uses std::rand(), which is messy but fast.
int quickRandInt(int upper);

// Returns a random integer between a and b.
// Uses std::rand(), which is messy but fast.
int quickRandRange(int a, int b);

// Returns a random integer from the interval [a, b).
// Uses a Mersenne Twister, which is slow but provides better randomness.
int randInt(int upper);

// Returns a random integer between a and b.
// Uses a Mersenne Twister, which is slow but provides better randomness.
int randRange(int a, int b);

#endif