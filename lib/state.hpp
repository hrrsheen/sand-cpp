#ifndef STATE_HPP
#define STATE_HPP

#include "grid.hpp"
#include "screen.hpp"

class State {
public:
    State(int width, int height);

    // Draws the grid to the screen.
    void draw(Screen &screen);

    CellType brushType;
    Grid grid;
};

#endif