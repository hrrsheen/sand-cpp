#ifndef WORLD_STATE_HPP
#define WORLD_STATE_HPP

#include "Cell.hpp"
#include "Chunks.hpp"
#include "Elements/ElementProperties.hpp"
#include "SandWorld.hpp"
#include "screen.hpp"
#include <vector>
#include <utility>

struct Brush {
    Element element;
    int size;
};

class WorldState {
public:
    SandWorld world;
    Brush brush;

private:
    

public:
    WorldState(int width, int height);

    // Performs one iteration of the simulation.
    void Step(float dt);

    // Draws the grid to the screen.
    void draw(Screen &screen);

private:
    void wakeAdjacentChunks(Cell &cell);

    void ApplyRules(Cell &cell, sf::Vector2i p);

    bool MoveCell(Cell &cell, ElementProperties &properties, sf::Vector2i p);
    bool SpreadCell(Cell &cell, ElementProperties &properties, sf::Vector2i p);
    bool ActionCell(Cell &cell, ElementProperties &properties, sf::Vector2i p);

    bool FloatDown(sf::Vector2i p);
    bool FloatUp(sf::Vector2i p);

    bool SpreadDownSide(sf::Vector2i p);
    bool SpreadUpSide(sf::Vector2i p);
    bool SpreadSide(sf::Vector2i p);
};

#endif