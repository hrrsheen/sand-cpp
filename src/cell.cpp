#include "cell.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>


Cell::Cell() : elementId(Element::air), p(0, 0), redraw(false), active(false) {
}

Cell::Cell(int x, int y, PropertiesContainer *properties) : 
    allProperties(properties), elementId(Element::air), 
    p(x, y), 
    redraw(false),
    active(false) {
    assign(elementId);
}

void Cell::assign(Element elementId) {
    Cell::elementId = elementId;
    ElementProperties &properties {allProperties->get(elementId)};
    if (properties.hasTexture()) {
        colour = properties.colourFromTexture(p);
    } else {
        colour = properties.colourFromArray();
    }
    redraw = true;
    active = properties.isMobile();
}

ElementProperties& Cell::properties() const {
    return allProperties->get(elementId);
}


void swap(Cell *cellA, Cell *cellB) {
    Cell tmpCell = *cellA;
    cellA->elementId = cellB->elementId;
    cellA->colour = cellB->colour;
    cellA->active = cellB->active;
    
    cellB->elementId = tmpCell.elementId;
    cellB->colour = tmpCell.colour;
    cellB->active = tmpCell.active;
}