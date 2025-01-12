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

struct Brush {
    Element element;
    int size;
};

class WorldState {
public:
    SandWorld world;
    Brush brush;

private:
    float dt;

    // For drawing the grid.
    sf::Image   gridImage;
    sf::Texture gridTexture;
    sf::Sprite  gridSprite;

public:
    WorldState(int width, int height);

    // Performs one iteration of the simulation.
    void Step(float dt);

    // Draws the grid to the screen.
    void Draw(Screen &screen);

private:
    void SimulateChunk(Chunk &chunk);

    // Returns true if the cell has performed some action.
    bool ApplyRules(Cell &cell, sf::Vector2i p);

    bool MoveCell(Cell &cell, ElementProperties &properties, sf::Vector2i p);
    bool SpreadCell(Cell &cell, ElementProperties &properties, sf::Vector2i p);
    bool ActionCell(Cell &cell, ElementProperties &properties, sf::Vector2i p);

    // Drawing functions.
    // Initialises the colour of the SandWorld grid to Black.
    void InitGridImage(int width, int height);
    // Updates the given cell to the given colour.
    void UpdateCell(int x, int y, sf::Color colour);
    void DrawWorld(Screen &screen);

};

#endif