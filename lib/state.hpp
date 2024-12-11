#ifndef STATE_HPP
#define STATE_HPP

#include "cell.hpp"
#include "chunks.hpp"
#include "elements/element_properties.hpp"
#include "grid.hpp"
#include "screen.hpp"
#include <vector>

struct Brush {
    Element element;
    int size;
};

class State {
public:
    State(int width, int height);

    // Performs one iteration of the simulation.
    void step(float dt);

    // Draws the grid to the screen.
    void draw(Screen &screen);

    Brush brush;
    Grid grid;

private:
    void applyRules(Cell &cell);

    void applySolidRules(Cell &cell, ElementProperties &properties);

    void applyLiquidRules(Cell &cell, ElementProperties &properties);

    void applyGasRules(Cell &cell, ElementProperties &properties);

    Chunks chunks;
};

#endif