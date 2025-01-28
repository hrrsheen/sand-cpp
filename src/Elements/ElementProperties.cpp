#include "Elements/ElementProperties.hpp"
#include "SandRoom.hpp"
#include "SandWorld.hpp"
#include "Utility/Random.hpp"
#include <cmath>
#include <fstream>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>

#define TEXTURE_INDEX 1

ConstProperties::ConstProperties() :
    type(ElementType::AIR), name(), moveBehaviour(MoveType::NONE), spreadBehaviour(SpreadType::NONE), actionSet() {}

ElementProperties::ElementProperties() : constants(Element::count + 1), colours(Element::count + 1), brushes(Element::count + 1) {
    ConstProperties constsInit;
    constsInit.type = ElementType::AIR;
    constsInit.name = "air";
    ColourProperties colourInit;
    COLOUR(colourInit.palette).push_back(0x000000ff);
    
    Insert(Element::air, constsInit, colourInit);
}

bool ElementProperties::Insert(Element id, ConstProperties consts, ColourProperties palette, PaintProperties brush) {
    if (Contains(id)) return false;

    constants[id] = consts;
    colours[id] = palette;
    brushes[id] = brush;
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Colouring.
//////////////////////////////////////////////////////////////////////////////////////////

sf::Color ElementProperties::Colour(Element id, int x, int y) const {
    if (HasTexture(id)) {
        return ColourFromTexture(id, x, y);
    } else {
        return ColourFromArray(id);
    }
}

sf::Color ElementProperties::ColourFromArray(Element id) const {
    // No palette defaults to black
    if (COLOUR(colours[id].palette).size() == 0) return sf::Color(0x00000000);

    // Element is a single colour.
    if (COLOUR(colours[id].palette).size() == 1) return sf::Color(COLOUR(colours[id].palette).at(0));

    int position {QuickRandInt(COLOUR(colours[id].palette).size())};
    return sf::Color(COLOUR(colours[id].palette).at(position));
}

sf::Color ElementProperties::ColourFromTexture(Element id, int x, int y) const {
    sf::Vector2u size {TEXTURE(colours[id].palette).getSize()};
    x = x % static_cast<int>(size.x);
    y = y % static_cast<int>(size.y);
    return TEXTURE(colours[id].palette).getPixel(std::abs(x), std::abs(y));
}

bool ElementProperties::HasTexture(Element id) const {
    return colours[id].palette.index() == TEXTURE_INDEX;
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Simulation.
//////////////////////////////////////////////////////////////////////////////////////////

// Action ElementProperties::ActUponSelf(sf::Vector2i p, Cell &self, float dt) const {
//     return Action::Null();
// }

// Action ElementProperties::ActUponOther(Cell &self,  ElementProperties &selfProp,
//                                        Cell &other, ElementProperties &otherProp,
//                                        sf::Vector2i p, sf::Vector2i otherP,
//                                        float dt) const {
//     return Action::Null();
// }

bool ElementProperties::CanDisplace(Element self, Element other) const {
    return CanDisplace(constants[self].type, constants[other].type);
}

bool ElementProperties::CanDisplace(ElementType self, ElementType other) const {
    switch (self) {
        case ElementType::SOLID:
            return (other != ElementType::SOLID);
        case ElementType::LIQUID:
            return (other != ElementType::SOLID && other != ElementType::LIQUID);
        case ElementType::GAS:
        case ElementType::AIR:
        default:
            return false;
    }
}

bool ElementProperties::Contains(Element id) const {
    return !constants[id].name.empty();
}
