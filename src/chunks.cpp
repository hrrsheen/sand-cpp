#include "Chunks.hpp"

Chunks::Chunks(int width, int height, int chunkSize) : 
    width(width), height(height), chunkSize(chunkSize), chunks(width * height, false) {
}

void Chunks::Set(int index, bool state) {
    chunks.at(index) = state;
}

void Chunks::Set(int x, int y, bool state) {
    Set(ToIndex(x, y), state);
}

void Chunks::SetContaining(int x, int y, bool state) {
    Set(Hash(x, y), state);
}

bool Chunks::IsActive(int index) const {
    return chunks.at(index);
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