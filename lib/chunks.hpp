#ifndef CHUNKS_HPP
#define CHUNKS_HPP

#include <vector>

struct Bounds {
    int x;
    int y;
    int size;
};

class Chunks {
public:
    Chunks(int width, int height, int chunkSize);

    // Sets the chunk that contains the given cell coordinates (x, y) to active.
    void setFromCell(int x, int y);

    // Sets the chunk that contains the given cell coordinates (x, y) to inactive.
    void clearFromCell(int x, int y);

    // Sets the given chunk to active.
    void set(int x, int y);

    // Sets the given chunk to inactive.
    void clear(int x, int y);

    // Returns whether the chunk at the given index is active.
    bool active(int index) const;

    // Returns whether the given chunk is active.
    bool active(int x, int y) const;

    // Returns the number of chunks.
    size_t size() const;

    // Returns the rectangle of points contained by the chunk at the given index.
    Bounds invHash(int index) const;

    const int width;
    const int height;

private:
    // Converts an (x, y) coordinate to a chunk number.
    int hash(int x, int y) const;

    const int chunkSize;

    std::vector<bool> chunks;
};

#endif