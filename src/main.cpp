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

bool shouldClose(sf::Event &event) {
    if (event.type == sf::Event::Closed)
        return true;
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape)
            return true;
    }

    return false;
}

MouseState getMouseState(sf::Event &event, MouseState currentState) {
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            return MouseState::drawing;
        }
    } else if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            return MouseState::idle;
        }
    // We want to stop drawing if the mouse leaves the frame.
    } else if (event.type == sf::Event::MouseLeft) {
        return MouseState::idle;
    }

    return currentState;
}

void getElementSelect(Brush &brush, sf::Event &event) {
    if (event.type == sf::Event::KeyPressed) {
        int number = event.key.code - 26;
        if (number >= 0 && number <= 4) {
            brush.element = static_cast<Element>(number);
        }
    }
}

void paint(sf::Vector2i position, WorldState &state) {
    if (state.brush.size == 1) {
        state.world.SetCell(position.x, position.y, state.brush.element);
    } else {
        state.world.SetArea(
            position.x - state.brush.size - 1,
            position.y - state.brush.size - 1,
            2 * state.brush.size - 1,
            2 * state.brush.size - 1,
            state.brush.element
        );
    }
}

int main() {
    InitRng();
    const int width {500}, height {500};
    sf::Clock clock;
    WorldState state {width, height};
    Screen screen {width, height, "Falling Sand"};
    screen.setTransform([height]{
        sf::Transformable transformation;
        transformation.setOrigin(0, height); // 1st transform - scale to world height.
        transformation.setScale(1.f, -1.f);  // 2nd transform - flip so that +y is up.
        return transformation.getTransform();
    }());
    screen.initGridImage(width, height);

    MouseState mState {idle};
    int elapsed {0};
    while (screen.isOpen())
    {
        sf::Time dt {clock.restart()};
        sf::Event event;

        while (screen.pollEvent(event))
        {
            if (shouldClose(event)) {
                screen.close();
                break;
            }

            mState = getMouseState(event, mState);
            getElementSelect(state.brush, event);
        }

        if (mState == MouseState::drawing) {
            sf::Vector2i mousePos {screen.mapPixelToCoords(sf::Mouse::getPosition(screen))};
            paint(mousePos, state);
        }
        state.Step(dt.asSeconds());
        state.draw(screen);
        // DEBUG ONLY - Draw the active chunks.
        for (int i = 0; i < state.world.chunks.Size(); i++) {
            if (state.world.chunks.IsActive(i)) {
                ChunkBounds bounds {state.world.chunks.GetBounds(i)};
                sf::RectangleShape rectangle;
                rectangle.setSize(sf::Vector2f(bounds.size, bounds.size));
                rectangle.setOutlineColor(sf::Color::Blue);
                rectangle.setOutlineThickness(1);
                rectangle.setFillColor(sf::Color::Transparent);
                rectangle.setPosition(bounds.x, bounds.y);
                screen.draw(rectangle, screen.renderStates);
            }
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