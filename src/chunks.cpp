#include "Chunks.hpp"

Chunks::Chunks(int width, int height, int chunkSize) : 
    width(width), height(height), chunkSize(chunkSize), chunks(width * height, ChunkState::ASLEEP) {
}

void Chunks::Set(int index, bool signal) {
    ChunkState &state {chunks.at(index)};
    switch (state) {
        case ChunkState::ASLEEP:
            if (signal) state = ChunkState::AWAKE;
            break;
        case ChunkState::STANDBY:
            if (signal) state = ChunkState::AWAKE;
            else        state = ChunkState::ASLEEP;
            break;
        case ChunkState::AWAKE:
            if (!signal) state = ChunkState::STANDBY;
            break;
    }
    chunks.at(index) = state;
}

void Chunks::Set(int x, int y, bool signal) {
    Set(ToIndex(x, y), signal);
}

void Chunks::SetContaining(int x, int y, bool state) {
    Set(Hash(x, y), state);
}

void Chunks::KeepNeighbourAlive(int x, int y) {
    const ChunkBounds bounds        {GetContainingBounds(x, y)};
    const sf::Vector2i chunkCoords  {ContainingChunk(x, y)};
    if (x == bounds.x && chunkCoords.x > 0) {
        Set(chunkCoords.x - 1, chunkCoords.y, true);
    } else if (x == bounds.x + bounds.size - 1 && chunkCoords.x < width - 1) {
        Set(chunkCoords.x + 1, chunkCoords.y, true);
    }

    if (y == bounds.y && chunkCoords.y > 0) {
        Set(chunkCoords.x, chunkCoords.y - 1, true);
    } else if (y == bounds.y + bounds.size - 1 && chunkCoords.y < height - 1) {
        Set(chunkCoords.x, chunkCoords.y + 1, true);
    }
}

bool Chunks::IsActive(int index) const {
    return chunks.at(index) != ChunkState::ASLEEP;
}

bool Chunks::IsActive(int x, int y) const {
    return IsActive(ToIndex(x, y));
}

bool Chunks::IsContainingActive(int x, int y) const {
    return IsActive(Hash(x, y));
}

sf::Vector2i Chunks::ContainingChunk(int x, int y) const {
    return sf::Vector2i(x / chunkSize, y / chunkSize);
}

size_t Chunks::Size() const {
    return width * height;
}

ChunkBounds Chunks::GetBounds(int index) const {
    int yChunk {index / width};
    int xChunk {index % width};

    return ChunkBounds{xChunk * chunkSize, yChunk * chunkSize, chunkSize};
}

ChunkBounds Chunks::GetContainingBounds(int x, int y) const {
    sf::Vector2i chunk {ContainingChunk(x, y)};
    return GetBounds(ToIndex(chunk.x, chunk.y));
}

int Chunks::Hash(int x, int y) const {
    return (y / chunkSize) * width + (x / chunkSize);
}

int Chunks::ToIndex(int x, int y) const {
    return x + y * width;
}