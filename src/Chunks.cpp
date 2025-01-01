#include "Chunks.hpp"
#include <cmath>

Chunk::Chunk() : 
    state       (false),
    nextState   (false),
    xMin(-1), yMin(-1), xMinW(-1), yMinW(-1),
    xMax(-1), yMax(-1), xMaxW(-1), yMaxW(-1) {
}

void Chunk::Reset(ChunkBounds bounds) {
    state       = false;
    nextState   = false;
    // Reset min; max.
    xMin  = bounds.x + bounds.size; xMax  = -1;
    xMinW = bounds.x + bounds.size; xMaxW = -1;
    yMin  = bounds.y + bounds.size; yMax  = -1;
    yMinW = bounds.y + bounds.size; yMaxW = -1;
}

void Chunk::KeepAlive(int x, int y, ChunkBounds bounds) {
    nextState = true;
    xMinW = std::clamp(std::min(x - 2, xMinW), bounds.x, bounds.x + bounds.size);
    xMaxW = std::clamp(std::max(x + 3, xMaxW), bounds.x, bounds.x + bounds.size);
    yMinW = std::clamp(std::min(y - 2, yMinW), bounds.y, bounds.y + bounds.size);
    yMaxW = std::clamp(std::max(y + 3, yMaxW), bounds.y, bounds.y + bounds.size);
}

void Chunk::Update(ChunkBounds bounds) {
    state       = nextState;
    nextState   = false;
    // Update current; reset working.
    xMin = xMinW; xMinW = bounds.x + bounds.size;
    yMin = yMinW; yMinW = bounds.y + bounds.size;
    xMax = xMaxW; xMaxW = -1;
    yMax = yMaxW; yMaxW = -1;
}

Chunks::Chunks(int width, int height, int chunkSize) : 
    width(width), height(height), chunkSize(chunkSize), chunks(width * height) {
    Reset();
}

void Chunks::Reset() {
    for (int i = 0; i < Size(); ++i) {
        ResetChunk(i);
    }
}

void Chunks::ResetChunk(int index) {
    ChunkBounds bounds {GetBounds(index)};
    GetChunk(index).Reset(bounds);
}

void Chunks::ResetChunk(int x, int y) {
    ResetChunk(ToIndex(x, y));
}

void Chunks::ResetContaining(int x, int y) {
    ResetChunk(Hash(x, y));
}

void Chunks::KeepContainingAlive(int x, int y) {
    ChunkBounds bounds {GetContainingBounds(x, y)};
    GetContainingChunk(x, y).KeepAlive(x, y, bounds);
}

void Chunks::KeepNeighbourAlive(int x, int y) {
    const ChunkBounds bounds        {GetContainingBounds(x, y)};
    const sf::Vector2i chunkCoords  {ContainingChunk(x, y)};
    if (x == bounds.x && chunkCoords.x > 0) {
        KeepContainingAlive(x - 1, y);
    } else if (x == bounds.x + bounds.size - 1 && chunkCoords.x < width - 1) {
        KeepContainingAlive(x + 1, y);
    }

    if (y == bounds.y && chunkCoords.y > 0) {
        KeepContainingAlive(x, y - 1);
    } else if (y == bounds.y + bounds.size - 1 && chunkCoords.y < height - 1) {
        KeepContainingAlive(x, y + 1);
    }
}

void Chunks::UpdateChunk(int index) {
    ChunkBounds bounds {GetBounds(index)};
    GetChunk(index).Update(bounds);
}

bool Chunks::IsActive(int index) const {
    return chunks.at(index).state;
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

ChunkBounds Chunks::GetBounds(int x, int y) const {
    return ChunkBounds{x * chunkSize, y * chunkSize, chunkSize};
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

Chunk& Chunks::GetChunk(int index) {
    return chunks.at(index);
}

Chunk& Chunks::GetChunk(int x, int y) {
    return GetChunk(ToIndex(x, y));
}

Chunk& Chunks::GetContainingChunk(int x, int y) {
    return GetChunk(Hash(x, y));
}

int Chunks::ToIndex(int x, int y) const {
    return x + y * width;
}