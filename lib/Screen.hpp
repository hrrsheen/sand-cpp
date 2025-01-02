#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <SFML/Graphics.hpp>

class Cell;
class Grid;
class Drawable;

class Screen : public sf::RenderWindow {
private:
    sf::RenderStates renderStates;
    sf::Transform tf;       // The transform from grid space to screen space.
    sf::Transform tfInv;    // The transform from screen space to grid space.

public:
    using sf::RenderWindow::draw;

    Screen(int width, int height, std::string name);

    void Draw(const sf::Drawable &drawable);

    // Updates the pixel at the given coordinates with the given colour.
    void UpdateCell(int x, int y, sf::Color);

    sf::Vector2f MapPixelToCoords(const sf::Vector2i &point) const;

    // Sets the transform to be used for translating from world space to screen space when drawing.
    void SetTransform(sf::Transform tf);    
};

#endif