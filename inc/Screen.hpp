#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <SFML/Graphics.hpp>

class Cell;
class Grid;
class Drawable;

class Screen : public sf::RenderWindow {
private:
    sf::RenderStates renderStates;
    sf::Transform tf;       // The transform from world space to screen space.
    sf::Transform tfInv;    // The transform from screen space to world space.

    sf::View view;          // The visible area of the screen (viewport).

public:
    using sf::RenderWindow::draw;

    Screen(int screenWidth, int screenHeight, int viewWidth, int viewHeight, std::string name);

    void Draw(const sf::Drawable &drawable);

    // Repositions the centre of the current view to the given position in world space.
    void RepositionView(sf::Vector2f delta);

    sf::Vector2f ToWorld(const sf::Vector2i &point) const;

    // Sets the transform to be used for translating from world space to screen space when drawing.
    void SetTransform(sf::Transform tf);    
};

#endif