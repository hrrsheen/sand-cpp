#include "Cell.hpp"
#include "Elements/ElementProperties.hpp"
#include "Elements/PropertiesContainer.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>


Cell::Cell() : elementId(Element::air), p(0, 0), redraw(false), active(false), velocity(0.f, 0.f) {
}

Cell::Cell(int x, int y, PropertiesContainer *properties) : 
    allProperties(properties), elementId(Element::air), 
    p(x, y), 
    redraw(false),
    active(false),
    velocity(0.f, 0.f) {
    Assign(elementId);
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Assignment / Manipulation functions.
//////////////////////////////////////////////////////////////////////////////////////////

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
    redrawEachFrame = properties.RecolourEachFrame();
}

void Cell::ApplyAcceleration(sf::Vector2f a, float dt) {
    velocity += a * dt;
    if (std::abs(velocity.x) > Cell::MAX_VELOCITY) velocity.x = Cell::MAX_VELOCITY;
    if (std::abs(velocity.y) > Cell::MAX_VELOCITY) velocity.y = Cell::MAX_VELOCITY;
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Get / Query functions.
//////////////////////////////////////////////////////////////////////////////////////////

ElementProperties& Cell::Properties() const {
    return allProperties->Get(elementId);
}

sf::Color Cell::Colour() {
    if (redrawEachFrame) {
        ElementProperties &properties {allProperties->Get(elementId)};
        colour = properties.ColourFromArray();
    }

    return colour;
}
