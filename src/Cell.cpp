#include "Cell.hpp"
#include "Constants.hpp"
#include "Elements/ElementProperties.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

void CellState::ApplyAcceleration(sf::Vector2f acc, float dt) {
    velocity += acc * dt;
    std::clamp(velocity.x, -constants::maxVelocity, constants::maxVelocity);
    std::clamp(velocity.y, -constants::maxVelocity, constants::maxVelocity);
}


Cells::Cells(int width, int height, const ElementProperties *_properties) : 
    properties(_properties),
    state(width * height, CellState()),
    colour(width * height, _properties->Colour(Element::air, 0, 0)) {}

//////////////////////////////////////////////////////////////////////////////////////////
//  Assignment / Manipulation functions.
//////////////////////////////////////////////////////////////////////////////////////////

void Cells::Assign(size_t i, Element _id, sf::Color newColour) {
    state[i]    = CellState(_id);
    colour[i]   = newColour;
}

void Cells::Assign(size_t i, Element _id, int x, int y) {
    Assign(i, _id, properties->Colour(_id, x, y));
}

void Cells::Darken(size_t i) {
    sf::Color &cellColour {colour[i]};
    cellColour.r = std::clamp(static_cast<int>((cellColour.r * 3.f) / 4.f), 25, 255);
    cellColour.g = std::clamp(static_cast<int>((cellColour.g * 3.f) / 4.f), 25, 255);
    cellColour.b = std::clamp(static_cast<int>((cellColour.b * 3.f) / 4.f), 25, 255);
}

const ConstProperties& Cells::GetProperties(int index) const {
    return properties->constants.at(state[index].id);
}

bool Cells::CanDisplace(Element self, Element other) const {
    return properties->CanDisplace(self, other);
}

int Cells::SpreadRate(size_t i) const {
    return properties->constants[state[i].id].spreadRate;
}

float Cells::Flammability(size_t i) const {
    return properties->constants[state[i].id].flammability;
}
