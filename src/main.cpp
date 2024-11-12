#include "screen.hpp"
#include "state.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

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

int main() {
    const int width {500}, height {500};
    sf::Clock clock;

    State state {width, height};
    Screen screen {width, height, "Falling Sand"};
    screen.setTransform([height]{
        sf::Transformable transformation;
        transformation.setOrigin(0, height); // 1st transform
        transformation.setScale(1.f, -1.f);  // 2nd transform
        return transformation.getTransform();
    }());

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
            if (mState == MouseState::drawing) {
                sf::Vector2i mousePos {screen.mapPixelToCoords(sf::Mouse::getPosition(screen))};
                state.grid.setCell(mousePos.x, mousePos.y, state.brushType);
                
            }
        }
        state.step(dt.asSeconds());
        state.draw(screen);

        if (elapsed >= 1000) {
            std::cout << "FPS: " << 1.f / dt.asSeconds() << "\n";
            elapsed = 0;
        } else {
            elapsed += dt.asMilliseconds();
        }
    }
}