#ifndef WORLD_STATE_HPP
#define WORLD_STATE_HPP

#include "Cell.hpp"
#include "Chunks.hpp"
#include "Elements/ElementProperties.hpp"
#include "FreeList.h"
#include "SandWorld.hpp"
#include "Screen.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <utility>

struct WorldDisplay {
    sf::Image   gridImage;
    sf::Texture gridTexture;
    sf::Sprite  gridSprite;

    WorldDisplay(int x, int y) {
        gridImage.create(constants::roomWidth, constants::roomHeight);
        gridTexture.create(constants::roomWidth, constants::roomHeight);
        gridTexture.setSmooth(false);
        gridSprite.setPosition(x, y);
    }
};

class WorldState {
public:
    SandWorld world;

private:
    FreeList<WorldDisplay> display; // The render information for each room.

public:
    WorldState(int width, int height);

    void Step(float dt);

    // Draws the grid to the screen.
    void Draw(Screen &screen);

private:
    std::vector<roomID_t> VisibleRooms(sf::IntRect);

};

#endif