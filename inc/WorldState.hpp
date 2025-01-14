#ifndef WORLD_STATE_HPP
#define WORLD_STATE_HPP

#include "Cell.hpp"
#include "Chunks.hpp"
#include "Elements/ElementProperties.hpp"
#include "SandWorld.hpp"
#include "Screen.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <utility>

class WorldState {
public:
    SandWorld world;

public:
    WorldState(int width, int height);

    void Step(float dt);

    // Draws the grid to the screen.
    void Draw(Screen &screen);

private:
    std::vector<roomID_t> VisibleRooms(sf::IntRect);

};

#endif