#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <SFML/System/Vector2.hpp>

namespace constants {
    const int roomWidth     = 512,  roomHeight      = 512;
    const int viewWidth     = 512,  viewHeight      = 256;
    const int screenWidth   = 1024, screenHeight    = 512;
    const int numXChunks    = 8,    numYChunks      = 8;
    const int chunkWidth    = 64,   chunkHeight     = 64;

    constexpr float maxVelocity     = 480.f;
    const sf::Vector2f accelGravity = {0.f, -60.f};
    const sf::Vector2f initialV     = {0.f, -240.f};

    const float M = 1e-2f;   // The particle mass coefficient.
    const float k = 1e-5f;   // The particle drag coefficient.
}

#endif