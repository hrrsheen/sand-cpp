#include "Cell.hpp"
#include "SandWorld.hpp"
#include "Screen.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

Screen::Screen(int width, int height, std::string name) : 
    sf::RenderWindow(sf::VideoMode(width, height), name),
    tf(),
    renderStates(sf::RenderStates::Default) {
    gridImage.create(width, height);
    gridTexture.create(width, height);
    gridTexture.setSmooth(false);
}

void Screen::InitGridImage(int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            gridImage.setPixel(x, y, sf::Color::Black);
        }
    }
}

void Screen::DrawWorld() {
    gridTexture.loadFromImage(gridImage);
    gridSprite.setTexture(gridTexture);
    sf::RenderWindow::draw(gridSprite, renderStates);
}

void Screen::Draw(const sf::Drawable &drawable) {
    sf::RenderWindow::draw(drawable, renderStates);
}

void Screen::UpdateCell(int x, int y, sf::Color colour) {
    gridImage.setPixel(x, y, colour);
}

sf::Vector2f Screen::MapPixelToCoords(const sf::Vector2i &point) const {
    return tfInv * sf::RenderWindow::mapPixelToCoords(point);
}

void Screen::SetTransform(sf::Transform tf) {
    Screen::tf = tf;
    tfInv = tf.getInverse();
    renderStates.transform = tf;
}
