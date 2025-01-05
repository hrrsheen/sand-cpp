#include "Helpers.hpp"
#include "Screen.hpp"
#include "WorldState.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <time.h>

#define KEY_TO_NUMBER(x) (x - sf::Keyboard::Num0)

enum MouseState {
    IDLE = 0,
    DRAWING,
    DRAGGING
};

struct Mouse {
    MouseState state;
    int size;               // The size of the brush.
    Element brush;          // The type of the brush.
    sf::Vector2i pos;       // The current position of the mouse, in window pixels.
    sf::Vector2i prevPos;   // The position of the mouse last frame, in window pixels.

    Mouse() : state(MouseState::IDLE), size(1), brush(Element::air), pos(sf::Vector2i(-1, -1)), prevPos(sf::Vector2i(-1, -1)) {}

    void Reset() {
        state   = MouseState::IDLE;
        pos     = sf::Vector2i(-1, -1);
        prevPos = sf::Vector2i(-1, -1);
    }

    void SetState(sf::Event &event, sf::Vector2i position) {
        if (event.type == sf::Event::MouseButtonPressed) {
            if      (event.mouseButton.button == sf::Mouse::Left  ) state = MouseState::DRAWING;
            else if (event.mouseButton.button == sf::Mouse::Middle) state = MouseState::DRAGGING;
            prevPos = position;
        } else if (event.type == sf::Event::MouseButtonReleased) {
            Reset();
        // We want to stop drawing if the mouse leaves the frame.
        } else if (event.type == sf::Event::MouseLeft) {
            Reset();
        } else if (event.type == sf::Event::MouseWheelMoved) {
            if      (event.mouseWheel.delta > 0) size = std::clamp(size + 1, 1, 10);
            else if (event.mouseWheel.delta < 0) size = std::clamp(size - 1, 1, 10);
        } else if (event.type == sf::Event::KeyPressed) {
            int number {KEY_TO_NUMBER(event.key.code)};
            if (number >= 0 && number <= Element::count - 1) {
                brush = static_cast<Element>(number);
            }
        }
    }
};

bool ShouldClose(sf::Event &event) {
    if (event.type == sf::Event::Closed)
        return true;
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape)
            return true;
    }

    return false;
}

void Paint(Lerp &stroke, Element type, int size, WorldState &state) {
    for (sf::Vector2i pos : stroke) {
        if (size == 1) {
            state.world.SetCell(pos.x, pos.y, type);
        } else {
            state.world.SetArea(
                pos.x - size - 1,
                pos.y - size - 1,
                2 * size - 1,
                2 * size - 1,
                type
            );
        }
    }
}

void Paint(Mouse &mouse, WorldState &state, Screen &screen) {
    sf::Vector2i end    {sf::Vector2i{screen.ToWorld(mouse.pos    )}};
    sf::Vector2i start  {sf::Vector2i{screen.ToWorld(mouse.prevPos)}};

    Lerp lerp {start, end};
    Paint(lerp, mouse.brush, mouse.size, state);
}

void DrawChunks(SandWorld &world, Screen &screen) {
    for (int i = 0; i < world.chunks.Size(); i++) {
        if (world.chunks.IsActive(i)) {
            Chunk &chunk {world.chunks.GetChunk(i)};
            ChunkBounds bounds {world.chunks.GetBounds(i)};
            sf::RectangleShape rectangle;
            rectangle.setSize(sf::Vector2f(bounds.size, bounds.size));
            rectangle.setOutlineColor(sf::Color::Blue);
            rectangle.setOutlineThickness(1);
            rectangle.setFillColor(sf::Color::Transparent);
            rectangle.setPosition(bounds.x, bounds.y);
            screen.Draw(rectangle);
            rectangle.setSize(sf::Vector2f(chunk.xMax - chunk.xMin, chunk.yMax - chunk.yMin));
            rectangle.setOutlineColor(sf::Color::Green);
            rectangle.setOutlineThickness(1);
            rectangle.setFillColor(sf::Color::Transparent);
            rectangle.setPosition(chunk.xMin, chunk.yMin);
            screen.Draw(rectangle);
        }
    }
}

int main() {
    bool DEBUG {false};
    InitRng();
    const int roomWidth {512}, roomHeight {512},
              viewWidth {512}, viewHeight {256};
    sf::Clock clock;
    WorldState state {roomWidth, roomHeight};
    Screen screen {viewWidth, viewHeight, "Falling Sand"};
    screen.SetTransform(
        [viewHeight] {
            sf::Transformable transformation;
            transformation.setOrigin(0, viewHeight);    // 1st transform - scale to world height.
            transformation.setScale(1.f, -1.f);         // 2nd transform - flip so that +y is up.
            return transformation.getTransform();
        }()
    );

    Mouse mouse;
    int elapsed {0};
    while (screen.isOpen()) {
        sf::Time dt {clock.restart()};
        sf::Event event;

        while (screen.pollEvent(event)) {
            if (ShouldClose(event)) {
                screen.close();
                break;
            }

            if (event.type == sf::Event::KeyPressed && event.key.scancode == sf::Keyboard::Scan::D) {
                DEBUG = !DEBUG;
            }

            mouse.SetState(event, sf::Mouse::getPosition(screen));
        }

        if (mouse.state) {
            mouse.pos = sf::Mouse::getPosition(screen);
            if (mouse.state == MouseState::DRAWING ) {
                Paint(mouse, state, screen);
            } else if (mouse.state == MouseState::DRAGGING) {
                // The view's position DOESN'T use the transformed world coordinates, so we need to use mapPixelToCoords.
                sf::Vector2f delta {screen.mapPixelToCoords(mouse.prevPos) - screen.mapPixelToCoords(mouse.pos)};
                screen.RepositionView(delta);
            }
            mouse.prevPos = mouse.pos;
        }
        state.Step(dt.asSeconds());
        state.Draw(screen);
        // DEBUG ONLY - Draw the active chunks.
        if (DEBUG) {
            DrawChunks(state.world, screen);
        }
        screen.display();

        if (elapsed >= 1000) {
            std::cout << "FPS: " << 1.f / dt.asSeconds() << "\n";
            elapsed = 0;
        } else {
            elapsed += dt.asMilliseconds();
        }
    }
}