#include "screen.hpp"
#include <SFML/Graphics.hpp>

Screen::Screen(int width, int height, std::string name) : 
    sf::RenderWindow(sf::VideoMode(width, height), name),
    tf(),
    renderStates(sf::RenderStates::Default) {
}

void Screen::draw(const sf::Drawable &drawable) {
    sf::RenderWindow::draw(drawable, renderStates);
}

sf::Vector2f Screen::mapPixelToCoords(const sf::Vector2i &point) const {
    return tfInv * sf::RenderWindow::mapPixelToCoords(point);
}

void Screen::setTransform(sf::Transform tf) {
    Screen::tf = tf;
    tfInv = tf.getInverse();
    renderStates.transform = tf;
}
