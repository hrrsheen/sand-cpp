#ifndef STATE_HPP
#define STATE_HPP

#include "grid.hpp"
#include "screen.hpp"

class State {
public:
    State(int width, int height);

    // Performs one iteration of the simulation.
    void step(float dt);

    // Draws the grid to the screen.
    void draw(Screen &screen);


    CellType brushType;
    Grid grid;

private:
    void applyRules(Cell &cell);
};

#endif