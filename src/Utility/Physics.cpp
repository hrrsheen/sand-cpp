#include "Utility/Physics.hpp"
#include "Utility/Random.hpp"
#include <cmath>

sf::Vector2i AccelerateProbability(sf::Vector2f velocity, float dt) {
    // If the resultant position a fractional part, use the fraction as a probability
    // to advance an additional cell.
    float xDst, yDst; // The destination.
    // Separate the fractional and whole part of the destination calculation.
    int xRem = static_cast<int>(100.f * std::modf(velocity.x * dt, &xDst));
    int yRem = static_cast<int>(100.f * std::modf(velocity.y * dt, &yDst));
    // Use the fractional part as a probability to advance and additional cell.
    if (Probability(std::abs(xRem))) xDst += (velocity.x > 0) - (velocity.x < 0);
    if (Probability(std::abs(yRem))) yDst += (velocity.y > 0) - (velocity.y < 0);

    // if (std::abs(velocity.x) > 0) velocity.x = xDst / dt;
    // if (std::abs(velocity.y) > 0) velocity.y = yDst / dt;

    return sf::Vector2i {static_cast<int>(xDst), static_cast<int>(yDst)};
}

sf::Vector2i AccelerationDistance(sf::Vector2f &velocity, float dt) {
    sf::Vector2f distance {velocity * dt};

    sf::Vector2i distRound {
        static_cast<int>(std::roundf(distance.x)),
        static_cast<int>(std::roundf(distance.y))
    };

    // velocity.x = static_cast<float>(distRound.x) / dt;
    // velocity.y = static_cast<float>(distRound.y) / dt;
    return distRound;
}