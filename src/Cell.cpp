#include "Cell.hpp"
#include "Constants.hpp"
#include "Elements/ElementProperties.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>


Cell::Cell() : id(Element::air), redraw(false), health(100.f), velocity(0.f, 0.f) {
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Assignment / Manipulation functions.
//////////////////////////////////////////////////////////////////////////////////////////

void Cell::Assign(Element _id, ElementProperties &properties, int x=0, int y=0) {
    id = _id;
    if (properties.HasTexture()) {
        colour = properties.ColourFromTexture(x, y);
    } else {
        colour = properties.ColourFromArray();
    }
    health          = 100.f;
    velocity        = sf::Vector2f(0.f, 0.f);
    redraw          = true;
}

void Cell::ApplyAcceleration(sf::Vector2f a, float dt) {
    velocity += a * dt;
    if (std::abs(velocity.x) > constants::maxVelocity) velocity.x = constants::maxVelocity;
    if (std::abs(velocity.y) > constants::maxVelocity) velocity.y = constants::maxVelocity;
}
