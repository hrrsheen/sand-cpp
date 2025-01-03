#include "Cell.hpp"
#include "SandWorld.hpp"
#include "Screen.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

Screen::Screen(int width, int height, std::string name) : 
    sf::RenderWindow(sf::VideoMode(1024, 512), name),
    renderStates(sf::RenderStates::Default),
    tf(), view(sf::FloatRect(0.f, 0.f, width, height)) {
    setView(view);
}

void Screen::Draw(const sf::Drawable &drawable) {
    sf::RenderWindow::draw(drawable, renderStates);
}

void Screen::RepositionView(sf::Vector2f delta) {
    const sf::Vector2f &centre {view.getCenter()};
}

sf::Vector2f Screen::MapPixelToCoords(const sf::Vector2i &point) const {
    return tfInv * sf::RenderWindow::mapPixelToCoords(point);
}

void Screen::SetTransform(sf::Transform tf) {
    Screen::tf = tf;
    tfInv = tf.getInverse();
    renderStates.transform = tf;
}
