#include "chunks.hpp"

Chunks::Chunks(int width, int height, int chunkSize) : 
    width(width), height(height), chunkSize(chunkSize), chunks(width * height, false) {
}

void Chunks::setFromCell(int x, int y) {
    chunks.at(hash(x, y)) = true;
}

void Chunks::clearFromCell(int x, int y) {
    chunks.at(hash(x, y)) = false;
}

void Chunks::set(int x, int y) {
    chunks.at(y * width + x) = true;
}

void Chunks::clear(int x, int y) {
    chunks.at(y * width + x) = false;
}

bool Chunks::active(int index) const {
    return chunks.at(index);
}

bool Chunks::active(int x, int y) const {
    return chunks.at(y * width + x);
}

size_t Chunks::size() const {
    return width * height;
}

Bounds Chunks::invHash(int index) const {
    int yChunk {index / width};
    int xChunk {index % width};

    return Bounds{xChunk * chunkSize, yChunk * chunkSize, chunkSize};
}

int Chunks::hash(int x, int y) const {
    return (y / chunkSize) * width + (x / chunkSize);
}