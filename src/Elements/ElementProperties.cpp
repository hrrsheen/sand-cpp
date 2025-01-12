#include "Elements.hpp"
#include "Helpers.hpp"
#include "SandRoom.hpp"
#include "SandWorld.hpp"
#include <cmath>
#include <fstream>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>

#define TEXTURE_INDEX 1

ConstProperties::ConstProperties() :
    type(ElementType::AIR), name(), moveBehaviour(MoveType::NONE), spreadBehaviour(SpreadType::NONE), actionSet(), 
    colourEachFrame(false),
    palette(),
    spreadRate(1),
    flammability(0.f) {}

ElementProperties::ElementProperties(Element _id, ConstProperties &inits) :
    id(_id),
    type            (inits.type),
    name            (inits.name),
    moveBehaviour   (inits.moveBehaviour),
    spreadBehaviour (inits.spreadBehaviour),
    actionSet       (inits.actionSet),
    colourEachFrame (inits.colourEachFrame),
    palette         (inits.palette),
    spreadRate      (inits.spreadRate),
    flammability    (inits.flammability) {}

//////////////////////////////////////////////////////////////////////////////////////////
//  Colouring.
//////////////////////////////////////////////////////////////////////////////////////////

sf::Color ElementProperties::ColourFromArray() {
    if (COLOUR(palette).size() == 0) {
        return sf::Color(0x00000000);
    }
    if (COLOUR(palette).size() == 1) {
        return sf::Color(COLOUR(palette).at(0)); // Trivial case where elements are a single colour.
    }
    int position {QuickRandInt(COLOUR(palette).size())};
    return sf::Color(COLOUR(palette).at(position));
}

sf::Color ElementProperties::ColourFromTexture(int x, int y) {
    sf::Vector2u size {TEXTURE(palette).getSize()};
    x = x % static_cast<int>(size.x);
    y = y % static_cast<int>(size.y);
    return TEXTURE(palette).getPixel(std::abs(x), std::abs(y));
}

bool ElementProperties::HasTexture() {
    return palette.index() == TEXTURE_INDEX;
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Simulation.
//////////////////////////////////////////////////////////////////////////////////////////

Action ElementProperties::ActUponSelf(sf::Vector2i p, Cell &self, float dt) const {
    return Action::Null();
}

Action ElementProperties::ActUponOther(Cell &self,  ElementProperties &selfProp,
                                       Cell &other, ElementProperties &otherProp,
                                       sf::Vector2i deltaP, float dt) const {
    return Action::Null();
}

bool ElementProperties::CanDisplace(ElementType other) const {
    return false;
}

bool ElementProperties::operator==(const ElementProperties &otherProperty) const {
    return id == otherProperty.id || name == otherProperty.name;
}
