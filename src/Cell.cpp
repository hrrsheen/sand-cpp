#include "Cell.hpp"
#include "Constants.hpp"
#include "Elements/ElementProperties.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>


Cell::Cell(const ElementProperties *_properties) : 
    id(Element::air), redraw(false), health(100.f), velocity(0.f, 0.f), 
    properties(_properties), colour(_properties->Colour(id, 0, 0)) {}

//////////////////////////////////////////////////////////////////////////////////////////
//  Assignment / Manipulation functions.
//////////////////////////////////////////////////////////////////////////////////////////

void Cell::Assign(Element _id, int x, int y) {
    id          = _id;
    colour      = properties->Colour(_id, x, y);
    health      = 100.f;
    velocity    = sf::Vector2f(0.f, 0.f);
    redraw      = true;
}

void Cell::ApplyAcceleration(sf::Vector2f a, float dt) {
    velocity += a * dt;
    if (std::abs(velocity.x) > constants::maxVelocity) velocity.x = constants::maxVelocity;
    if (std::abs(velocity.y) > constants::maxVelocity) velocity.y = constants::maxVelocity;
}

bool Cell::CanDisplace(Element other) const {
    return properties->CanDisplace(id, other);
}

int Cell::SpreadRate() const {
    return properties->constants[id].spreadRate;
}

int Cell::Flammability() const {
    return properties->constants[id].flammability;
}
