#ifndef CELL_HPP
#define CELL_HPP

#include "Elements/Names.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

struct ElementProperties;

class Cell {
public:
    Element id;         // The ID used for accessing properties from the container.
    
    bool redraw;        // Tracks whether this cell has changes since the previous frame.
    
    float health;
    sf::Vector2f velocity;

    sf::Color colour;       // The colour of this cell.

    Cell();

    //////// Assignment / manipulation functions ////////
    void Assign(Element id, const ElementProperties &properties, int x=0, int y=0);

    void ApplyAcceleration(sf::Vector2f a, float dt);

    
};

#endif