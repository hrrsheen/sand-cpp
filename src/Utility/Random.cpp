#include "Utility/Random.hpp"
#include <random>



std::mt19937 rng(std::random_device{}());

void InitRng() {
    std::srand(std::random_device{}());
    rng.discard(10000);
}

int QuickRandInt(int upper) {
    if (upper) {
        return std::rand() % upper;
    }
    
    return 0;
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