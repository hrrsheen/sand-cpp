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

#define KEY_TO_NUMBER(x) (x - sf::Keyboard::Num0)

class Lerp;

enum MouseState {
    IDLE = 0,
    DRAWING,
    DRAGGING
};

struct Mouse {
    MouseState state;
    int radius;             // The size of the brush.
    Element brush;          // The type of the brush.
    sf::Vector2i pos;       // The current position of the mouse, in window pixels.
    sf::Vector2i prevPos;   // The position of the mouse last frame, in window pixels.

    PaintProperties brushInfo;

    Mouse();

    void Reset();
};

class SandGame {
    SandWorld world;
    const int xMinRooms, xMaxRooms,
              yMinRooms, yMaxRooms;

    Screen      screen;
    sf::Image   gridImage;
    sf::Texture gridTexture;
    sf::Sprite  gridSprite;
    // FPS display.
    sf::Font    font;
    sf::Text    text;

    // Contains the room ID of each view corner. Will always be ordered BL -> BR -> TL -> TR.
    std::vector<std::pair<sf::Vector2i, roomID_t>> visibleRooms;

public:
    SandGame();
    void Close() { screen.close(); }
    void Run();

private:
    void Step(float dt);

    // Game interaction
    void SetMouseState(Mouse &mouse, sf::Event &event, sf::Vector2i position);
    void Paint(Mouse &mouse);
    void Paint(Lerp &stroke, Element type, int radius);

    // Moves the view based on the mouse state.
    void RepositionView(Mouse mouse);

    // Draws visible area of the world to the screen.
    void Draw(Screen &screen);
    
    // Updates the member vector (visibleRooms) that contains the IDs of each room that is currently visible in the view.
    void UpdateVisibleRooms();

    // DEBUGGING.
    void DrawChunks();
};

#endif