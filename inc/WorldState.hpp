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

class WorldState {
public:
    SandWorld world;

private:
    sf::Image   gridImage;
    sf::Texture gridTexture;
    sf::Sprite  gridSprite;

public:
    WorldState(int width, int height);

    void Step(float dt);

    // Draws visible area of the world to the screen.
    void Draw(Screen &screen);

private:
    // Draws an area of the given room to its texture.
    void DrawArea(Screen &screen, SandRoom &room, 
                    int left, int right, int bottom, int top);

    // Returns a vector of pairs that contain a view corner and the room the corner intersects with.
    std::vector<std::pair<sf::Vector2i, roomID_t>> VisibleRooms(sf::IntRect);

};

#endif