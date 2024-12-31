#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <SFML/Graphics.hpp>

class Cell;
class Grid;

class Screen : public sf::RenderWindow {
public:
    using sf::RenderWindow::draw;

    Screen(int width, int height, std::string name);

    // Initialises the vertex array to reflect the size of the grid.
    void InitGridImage(int width, int height);

    void Draw();

    void AddCell(sf::Vector2f anchor, float width, sf::Color colour);

    // Updates the pixel at the given coordinates with the given colour.
    void UpdateCell(int x, int y, sf::Color);

    sf::Vector2f MapPixelToCoords(const sf::Vector2i &point) const;

    // Sets the transform to be used for translating from world space to screen space when drawing.
    void SetTransform(sf::Transform tf);

    sf::RenderStates renderStates; // TODO Move to private. Public for DEBUGGING only.
private:
    sf::Transform tf;
    sf::Transform tfInv;
    

    // For drawing the grid.
    sf::Image gridImage;
    sf::Texture gridTexture;
    sf::Sprite gridSprite;
};

#endif