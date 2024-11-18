#include "cell.hpp"
#include "grid.hpp"
#include "screen.hpp"
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

void Screen::initGridImage(Grid &grid) {
    for (int i = 0; i < grid.size(); i++) {
        Cell &cell {grid.getCell(i)};
        gridImage.setPixel(cell.p.x, cell.p.y, cell.colour);
    }
}

void Screen::draw() {
    gridTexture.loadFromImage(gridImage);
    gridSprite.setTexture(gridTexture);
    sf::RenderWindow::draw(gridSprite, renderStates);
}

void Screen::updateCell(Cell &cell, int index) {
    gridImage.setPixel(cell.p.x, cell.p.y, cell.colour);
}

sf::Vector2f Screen::mapPixelToCoords(const sf::Vector2i &point) const {
    return tfInv * sf::RenderWindow::mapPixelToCoords(point);
}

void Screen::setTransform(sf::Transform tf) {
    Screen::tf = tf;
    tfInv = tf.getInverse();
    renderStates.transform = tf;
}
