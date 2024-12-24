#include "Elements/ElementProperties.hpp"
#include "Helpers.hpp"
#include <cmath>
#include <fstream>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>

////////////////////////////////////////////////////////// Cell Properties //////////////////////////////////////////////////////////
ElementProperties::ElementProperties() : 
    id(Element::null), 
    type(),
    name(),
    moveBehaviour(MoveType::NONE), 
    spreadBehaviour(SpreadType::NONE) {
}

ElementProperties::ElementProperties(ElementType thisType) : 
    id(), 
    type(thisType), 
    name(), 
    moveBehaviour(MoveType::NONE), 
    spreadBehaviour(SpreadType::NONE) {}

ElementProperties::ElementProperties(Element thisId, ElementType thisType, std::string_view thisName) :
    id(thisId), type(thisType), name(thisName), moveBehaviour(MoveType::NONE), spreadBehaviour(SpreadType::NONE) {}

ElementProperties::ElementProperties(Element thisId, ElementType thisType, std::string_view thisName, MoveType movement, uint8_t spread) :
    id(thisId), type(thisType), name(thisName), moveBehaviour(movement), spreadBehaviour(spread) {}

sf::Color ElementProperties::ColourFromArray() {
    if (std::get<colourIndex>(palette).size() == 0) {
        return sf::Color(0x00000000);
    }
    if (std::get<colourIndex>(palette).size() == 1) {
        return sf::Color(std::get<colourIndex>(palette).at(0)); // Trivial case where elements are a single colour.
    }
    int position {QuickRandInt(std::get<colourIndex>(palette).size())};
    return sf::Color(std::get<colourIndex>(palette).at(position));
}

sf::Color ElementProperties::ColourFromTexture(sf::Vector2i p) {
    sf::Vector2u size {std::get<textureIndex>(palette).getSize()};
    int x {p.x % static_cast<int>(size.x)};
    int y {p.y % static_cast<int>(size.y)};
    return std::get<textureIndex>(palette).getPixel(std::abs(x), std::abs(y));
}

bool ElementProperties::HasTexture() {
    return palette.index() == textureIndex;
}

bool ElementProperties::canDisplace(ElementProperties &other) const {
    return false;
}

bool ElementProperties::operator==(const ElementProperties &otherProperty) const {
    return id == otherProperty.id || name == otherProperty.name;
}
