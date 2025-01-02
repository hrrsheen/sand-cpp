#include "Cell.hpp"
#include "SandWorld.hpp"
#include "Screen.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

Screen::Screen(int width, int height, std::string name) : 
    sf::RenderWindow(sf::VideoMode(width, height), name),
    tf(),
    renderStates(sf::RenderStates::Default) {
}

void Screen::Draw(const sf::Drawable &drawable) {
    sf::RenderWindow::draw(drawable, renderStates);
}

sf::Vector2f Screen::MapPixelToCoords(const sf::Vector2i &point) const {
    return tfInv * sf::RenderWindow::mapPixelToCoords(point);
}

void Screen::SetTransform(sf::Transform tf) {
    Screen::tf = tf;
    tfInv = tf.getInverse();
    renderStates.transform = tf;
}
