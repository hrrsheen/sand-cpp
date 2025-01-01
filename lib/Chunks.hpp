#ifndef CHUNKS_HPP
#define CHUNKS_HPP

#include <SFML/System/Vector2.hpp>
#include <vector>

struct ChunkBounds {
    int x;
    int y;
    int size;
};

enum ChunkState {
    ASLEEP,
    STANDBY,
    AWAKE
};

class Chunk {
public:
    bool state;
    bool nextState;
    int xMin, yMin,
        xMax, yMax;     // Dirty rect.
private:
    int xMinW, yMinW,
        xMaxW, yMaxW;   // Working dirty rect.
public:
    Chunk();

    void Reset(ChunkBounds bounds);
    // Sets the state to AWAKE and updates the dirty-rect based on the given (x, y) coordinates.
    void KeepAlive(int x, int y, ChunkBounds bounds);

    void Update(ChunkBounds bounds);
};

class Chunks {
public:
    const int width;
    const int height;

private:
    const int chunkSize;
    std::vector<Chunk> chunks;

public:
    Chunks(int width, int height, int chunkSize);

    void Reset();
    void ResetChunk(int index);
    void ResetChunk(int x, int y);
    void ResetContaining(int x, int y);

    // Setting functions.
    // Keeps the chunk that contains the point (x, y) alive and updates its dirty-rect.
    void KeepContainingAlive(int x, int y);
    // Sets the state of the neighboring chunk alive if (x, y) is on the border.
    void KeepNeighbourAlive(int x, int y);

    // Update functions.
    void UpdateChunk(int index);

    // Query functions.
    Chunk& GetChunk(int index);
    Chunk& GetChunk(int x, int y);
    Chunk& GetContainingChunk(int x, int y);

    bool IsActive(int index) const;
    bool IsActive(int x, int y) const;
    bool IsContainingActive(int x, int y) const;

    // Returns the coordinate of the chunk that contains the given (x, y) point.
    sf::Vector2i ContainingChunk(int x, int y) const;

    // Returns the number of chunks.
    size_t Size() const;

    // Retrieving chunk boundaries.
    ChunkBounds GetBounds(int x, int y) const;
    ChunkBounds GetBounds(int index) const;
    // Returns the boundaries of the chunk that contains the given (x, y) point.
    ChunkBounds GetContainingBounds(int x, int y) const;

private:
    // Converts an (x, y) coordinate in world space to a chunk index.
    int Hash(int x, int y) const;

    // Converts a chunk (x, y) index to a flat index.
    int ToIndex(int x, int y) const;
    sf::Vector2i ToCoords(int index) const;
};

#endif