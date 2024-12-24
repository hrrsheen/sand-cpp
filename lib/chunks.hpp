#ifndef CHUNKS_HPP
#define CHUNKS_HPP

#include <SFML/System/Vector2.hpp>
#include <vector>

struct ChunkBounds {
    int x;
    int y;
    int size;
};

class Chunks {
public:
    const int width;
    const int height;

private:
    const int chunkSize;
    std::vector<bool> chunks;

public:
    Chunks(int width, int height, int chunkSize);

    // Setting functions.
    void Set(int index, bool state);
    void Set(int x, int y, bool state);
    // Cets the state of the chunk containing coordinates (x, y).
    void SetContaining(int x, int y, bool state);

    // Query functions.
    bool IsActive(int index) const;
    bool IsActive(int x, int y) const;
    bool IsContainingActive(int x, int y) const;

    // Returns the coordinate of the chunk that contains the given (x, y) point.
    sf::Vector2i ContainingChunk(int x, int y) const;

    // Returns the number of chunks.
    size_t Size() const;

    // Retrieving chunk boundaries.
    ChunkBounds GetBounds(int index) const;
    // Returns the boundaries of the chunk that contains the given (x, y) point.
    ChunkBounds GetContainingBounds(int x, int y) const;

private:
    // Converts an (x, y) coordinate to a chunk index.
    int Hash(int x, int y) const;

    int ToIndex(int x, int y) const;
    sf::Vector2i ToCoords(int index) const;
};

#endif