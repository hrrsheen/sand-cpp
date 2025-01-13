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

private:
    // For drawing the grid.
    sf::Image   gridImage;
    sf::Texture gridTexture;
    sf::Sprite  gridSprite;

public:
    WorldState(int width, int height);

    void Step(float dt);

    // Draws the grid to the screen.
    void Draw(Screen &screen);

private:

    // Drawing functions.
    // Initialises the colour of the SandWorld grid to Black.
    void InitGridImage(int width, int height);
    // Updates the given cell to the given colour.
    void UpdateCell(int x, int y, sf::Color colour);
    void DrawWorld(Screen &screen);

};

#endif