#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <SFML/Graphics.hpp>

class Screen : public sf::RenderWindow {
public:
    Screen(int width, int height, std::string name);

    void draw();

    void addCell(sf::Vector2f anchor, float width, sf::Color colour);

    sf::Vector2f mapPixelToCoords(const sf::Vector2i &point) const;

    void setTransform(sf::Transform tf);

private:
    sf::Transform tf;
    sf::Transform tfInv;
    sf::RenderStates renderStates;

    sf::VertexArray vertices;
};

#endif