#ifndef CELL_HPP
#define CELL_HPP

#include "Elements/ElementProperties.hpp"
#include "Elements/PropertiesContainer.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

class Cell {
public:
    Element elementId;  // The ID used for accessing properties from the container.
    sf::Vector2i p;     // The (x, y) coordinate of this cell within its grid.
    
    bool redraw;        // Tracks whether this cell has changes since the previous frame.
    bool active;        // Tracks whether the cell is currently being simulated, or is at rest.
    
    float health;
    sf::Vector2f velocity;

private:
    sf::Color colour;       // The colour of this cell.
    bool redrawEachFrame;   // Flag for whether a new colour is selected each frame.

    PropertiesContainer *allProperties;

    static constexpr float MAX_VELOCITY = 9000.f;

public:
    Cell();
    Cell(int x, int y, PropertiesContainer *properties);

    //////// Assignment / manipulation functions ////////
    void Assign(Element elementId);

    void ApplyAcceleration(sf::Vector2f a, float dt);

    //////// Get / Query functions ////////
    ElementProperties& Properties() const;

    sf::Color Colour();
};

#endif