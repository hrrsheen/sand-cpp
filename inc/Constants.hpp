#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

namespace constants {
    const int roomWidth     = 512,  roomHeight      = 512;
    const int viewWidth     = 512,  viewHeight      = 256;
    const int screenWidth   = 1024, screenHeight    = 512;
    const int numXChunks    = 8,    numYChunks      = 8;
    const int chunkWidth    = 64,   chunkHeight     = 64;

    constexpr float accelGravity    = 2000.f;
    constexpr float maxVelocity     = 9000.f;
}

#endif