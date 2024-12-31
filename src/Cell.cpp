#include "Cell.hpp"
#include "Elements/ElementProperties.hpp"
#include "Elements/PropertiesContainer.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>


Cell::Cell() : elementId(Element::air), redraw(false), active(false), health(100.f), velocity(0.f, 0.f) {
}

Cell::Cell(int x, int y, PropertiesContainer *properties) : 
    allProperties(properties), elementId(Element::air), 
    redraw(false),
    active(false),
    health(100.f),
    velocity(0.f, 0.f) {
    Assign(elementId, x, y);
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Assignment / Manipulation functions.
//////////////////////////////////////////////////////////////////////////////////////////

void Cell::Assign(Element elementId, int x, int y) {
    Cell::elementId = elementId;
    ElementProperties &properties {allProperties->Get(elementId)};
    if (properties.HasTexture()) {
        colour = properties.ColourFromTexture(x, y);
    } else {
        colour = properties.ColourFromArray();
    }
    health          = 100.f;
    velocity        = sf::Vector2f(0.f, 0.f);
    active          = properties.moveBehaviour != MoveType::NONE;
    redraw          = true;
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
