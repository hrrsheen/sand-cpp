#ifndef CELL_HPP
#define CELL_HPP

#include "elements/element_properties.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

class Cell {
public:
    Cell();
    Cell(int x, int y, PropertiesContainer *properties);

    void assign(Element elementId);
    ElementProperties& properties() const;

    friend void swap(Cell *, Cell *);

    Element elementId;  // The ID used for accessing properties from the container.
    sf::Vector2i p;     // The (x, y) coordinate of this cell within its grid.
    sf::Color colour;   // The colour of this cell.

    bool redraw;        // Tracks whether this cell has changes since the previous frame.
    bool active;        // Tracks whether the cell is currently being simulated, or is at rest.

private:
    PropertiesContainer *allProperties;
};

// Swaps the cell types of cellA and cellB.
void swap(Cell *cellA, Cell *cellB);

#endif