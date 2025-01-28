#ifndef UTILITY_RANDOM_HPP
#define UTILITY_RANDOM_HPP

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

#endif