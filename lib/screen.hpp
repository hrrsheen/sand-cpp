#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <SFML/Graphics.hpp>

class Cell;
class Grid;

class Screen : public sf::RenderWindow {
public:
    Screen(int width, int height, std::string name);

    // Initialises the vertex array to reflect the size of the grid.
    void initGridImage(Grid &grid);

    void draw();

    void addCell(sf::Vector2f anchor, float width, sf::Color colour);

    // Updates the vertex at the given index in the VertexArray to display the given cell.
    void updateCell(Cell &cell, int index);

    sf::Vector2f mapPixelToCoords(const sf::Vector2i &point) const;

    // Sets the transform to be used for translating from world space to screen space when drawing.
    void setTransform(sf::Transform tf);

private:
    sf::Transform tf;
    sf::Transform tfInv;
    sf::RenderStates renderStates;

    // For drawing the grid.
    sf::Image gridImage;
    sf::Texture gridTexture;
    sf::Sprite gridSprite;
};

#endif