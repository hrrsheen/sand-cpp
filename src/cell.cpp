#include "Cell.hpp"
#include "Elements/ElementProperties.hpp"
#include "Elements/PropertiesContainer.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>


Cell::Cell() : elementId(Element::air), p(0, 0), redraw(false), active(false) {
}

Cell::Cell(int x, int y, PropertiesContainer *properties) : 
    allProperties(properties), elementId(Element::air), 
    p(x, y), 
    redraw(false),
    active(false) {
    Assign(elementId);
}

void Cell::Assign(Element elementId) {
    Cell::elementId = elementId;
    ElementProperties &properties {allProperties->Get(elementId)};
    if (properties.HasTexture()) {
        colour = properties.ColourFromTexture(p);
    } else {
        colour = properties.ColourFromArray();
    }
    redraw = true;
    active = properties.moveBehaviour != MoveType::NONE;
}

ElementProperties& Cell::Properties() const {
    return allProperties->Get(elementId);
}


void swap(Cell *cellA, Cell *cellB) {
    Cell tmpCell = *cellA;
    cellA->elementId = cellB->elementId;
    cellA->colour = cellB->colour;
    cellA->p = cellB->p;
    cellA->active = cellB->active;
    
    cellB->elementId = tmpCell.elementId;
    cellB->colour = tmpCell.colour;
    // cellB->p = tmpCell.p;
    cellB->active = tmpCell.active;
}