#include "Cell.hpp"
#include "SandWorld.hpp"
#include "Screen.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

Screen::Screen(int screenWidth, int screenHeight, int viewWidth, int viewHeight, std::string name) : 
    sf::RenderWindow(sf::VideoMode(screenWidth, screenHeight), name),
    renderStates(sf::RenderStates::Default),
    tf(), view(sf::FloatRect(0.f, 0.f, viewWidth, viewHeight)) {
    setView(view);
}

void Screen::Draw(const sf::Drawable &drawable) {
    sf::RenderWindow::draw(drawable, renderStates);
}

const sf::FloatRect Screen::ViewBorders() const {
    return sf::FloatRect(view.getCenter(), view.getSize());
}

void Screen::RepositionView(sf::Vector2f delta) {
    const sf::Vector2f &centre {view.getCenter()};

    view.setCenter(centre + delta);
    setView(view);
}

sf::Vector2f Screen::ToWorld(const sf::Vector2i &point) const {
    return tfInv * sf::RenderWindow::mapPixelToCoords(point);
}

void Screen::SetTransform(sf::Transform tf) {
    Screen::tf = tf;
    tfInv = tf.getInverse();
    renderStates.transform = tf;
}
