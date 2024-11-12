#include "screen.hpp"
#include <SFML/Graphics.hpp>

Screen::Screen(int width, int height, std::string name) : 
    sf::RenderWindow(sf::VideoMode(width, height), name),
    tf(),
    renderStates(sf::RenderStates::Default),
    vertices(sf::Triangles) {
}

void Screen::draw() {
    sf::RenderWindow::draw(vertices, renderStates);
    vertices.clear();
}

void Screen::addCell(sf::Vector2f anchor, float width, sf::Color colour) {
    sf::Vertex v0(anchor, colour);
    sf::Vertex v1(anchor + sf::Vector2f(0.f, width), colour);
    sf::Vertex v2(anchor + sf::Vector2f(width, 0.f), colour);
    sf::Vertex v3(anchor + sf::Vector2f(width, width), colour);

    // Append the lower triangle.
    vertices.append(v0);
    vertices.append(v1);
    vertices.append(v2);

    // Append the upper triangle.
    vertices.append(v1);
    vertices.append(v2);
    vertices.append(v3);
}

sf::Vector2f Screen::mapPixelToCoords(const sf::Vector2i &point) const {
    return tfInv * sf::RenderWindow::mapPixelToCoords(point);
}

void Screen::setTransform(sf::Transform tf) {
    Screen::tf = tf;
    tfInv = tf.getInverse();
    renderStates.transform = tf;
}
