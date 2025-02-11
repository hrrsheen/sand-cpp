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


Cells::Cells(int width, int height) : state(width * height, CellState()), colour(width * height) {}

//////////////////////////////////////////////////////////////////////////////////////////
//  Assignment / Manipulation functions.
//////////////////////////////////////////////////////////////////////////////////////////

void Cells::Assign(size_t i, Element id, sf::Color newColour) {
    state[i]    = CellState(id);
    colour[i]   = newColour;
}

void Cells::Darken(size_t i) {
    sf::Color &cellColour {colour[i]};
    cellColour.r = std::clamp(static_cast<int>((cellColour.r * 3.f) / 4.f), 25, 255);
    cellColour.g = std::clamp(static_cast<int>((cellColour.g * 3.f) / 4.f), 25, 255);
    cellColour.b = std::clamp(static_cast<int>((cellColour.b * 3.f) / 4.f), 25, 255);
}
