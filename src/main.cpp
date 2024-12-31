#include "Helpers.hpp"
#include "screen.hpp"
#include "WorldState.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <time.h>

enum MouseState {
    idle,
    drawing
};

struct Mouse {
    MouseState state;
    sf::Vector2i pos;
    sf::Vector2i prevPos;

    void Reset() {
        state   = MouseState::idle;
        pos     = sf::Vector2i(-1, -1);
        prevPos = sf::Vector2i(-1, -1);
    }

    void SetState(sf::Event &event) {
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                state = MouseState::drawing;
            }
        } else if (event.type == sf::Event::MouseButtonReleased) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                Reset();
            }
        // We want to stop drawing if the mouse leaves the frame.
        } else if (event.type == sf::Event::MouseLeft) {
            Reset();
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

MouseState GetMouseState(sf::Event &event, MouseState currentState) {
    

    return currentState;
}

void GetElementSelect(Brush &brush, sf::Event &event, SandWorld &world) {
    if (event.type == sf::Event::KeyPressed) {
        int number = event.key.code - 26;
        if (number >= 0 && number <= world.PropertiesSize() - 1) {
            brush.element = static_cast<Element>(number);
        }
    }
}

void Paint(Lerp &stroke, WorldState &state) {
    for (sf::Vector2i pos : stroke) {
        if (state.brush.size == 1) {
        state.world.SetCell(pos.x, pos.y, state.brush.element);
        } else {
            state.world.SetArea(
                pos.x - state.brush.size - 1,
                pos.y - state.brush.size - 1,
                2 * state.brush.size - 1,
                2 * state.brush.size - 1,
                state.brush.element
            );
        }
    }
}

void Paint(Mouse &mouse, WorldState &state) {
    sf::Vector2i start {mouse.prevPos};
    if (start == sf::Vector2i(-1, -1)) {
        start = mouse.pos;
    }

    Lerp lerp {start, mouse.pos};
    Paint(lerp, state);
}

void DrawChunks(SandWorld &world, Screen &screen) {
    for (int i = 0; i < world.chunks.Size(); i++) {
        if (world.chunks.IsAwake(i)) {
            Chunk &chunk {world.chunks.GetChunk(i)};
            ChunkBounds bounds {world.chunks.GetBounds(i)};
            sf::RectangleShape rectangle;
            rectangle.setSize(sf::Vector2f(bounds.size, bounds.size));
            rectangle.setOutlineColor(sf::Color::Blue);
            rectangle.setOutlineThickness(1);
            rectangle.setFillColor(sf::Color::Transparent);
            rectangle.setPosition(bounds.x, bounds.y);
            screen.draw(rectangle, screen.renderStates);
            rectangle.setSize(sf::Vector2f(chunk.xMax - chunk.xMin, chunk.yMax - chunk.yMin));
            rectangle.setOutlineColor(sf::Color::Green);
            rectangle.setOutlineThickness(1);
            rectangle.setFillColor(sf::Color::Transparent);
            rectangle.setPosition(chunk.xMin, chunk.yMin);
            screen.draw(rectangle, screen.renderStates);
        }
    }
}

int main() {
    bool DEBUG {false};
    InitRng();
    const int width {500}, height {500};
    sf::Clock clock;
    WorldState state {width, height};
    Screen screen {width, height, "Falling Sand"};
    screen.SetTransform([height]{
        sf::Transformable transformation;
        transformation.setOrigin(0, height); // 1st transform - scale to world height.
        transformation.setScale(1.f, -1.f);  // 2nd transform - flip so that +y is up.
        return transformation.getTransform();
    }());
    screen.InitGridImage(width, height);

    Mouse mouse;
    mouse.Reset();
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

            mouse.SetState(event);
            GetElementSelect(state.brush, event, state.world);
        }

        if (mouse.state == MouseState::drawing) {
            mouse.pos = sf::Vector2i{screen.MapPixelToCoords(sf::Mouse::getPosition(screen))};
            Paint(mouse, state);
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