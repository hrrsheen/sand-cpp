#include <random>

std::mt19937 rng(std::random_device{}());

void initRng() {
    std::srand(std::random_device{}());
    rng.discard(10000);
}

int quickRandInt(int upper) {
    return std::rand() % upper;
}

int quickRandRange(int a, int b) {
    return (std::rand() / static_cast<float>(RAND_MAX)) * (b - a) + a;
}

int randInt(int upper) {
    return std::rand() % upper;
}

int randRange(int a, int b) {
    return (std::rand() / static_cast<float>(RAND_MAX)) * (b - a) + a;
}
