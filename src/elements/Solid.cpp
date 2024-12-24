#include "Elements/ElementProperties.hpp"
#include "Elements/Solid.hpp"

Solid::Solid() : ElementProperties(ElementType::SOLID) {}
Solid::Solid(Element thisId, std::string_view thisName, MoveType move, uint8_t spread) : 
    ElementProperties(thisId, ElementType::SOLID, thisName, move, spread) {}

Sand::Sand() : Solid(Element::sand, "sand", MoveType::FLOAT_DOWN, SpreadType::DOWN_SIDE) {
    std::get<ElementProperties::colourIndex>(palette).push_back(0xfabf73ff);
    std::get<ElementProperties::colourIndex>(palette).push_back(0xebae60ff);
}

Stone::Stone() : Solid(Element::stone, "stone", MoveType::NONE, SpreadType::NONE) {
    sf::Image img;
    img.loadFromFile("./assets/stone-texture.png");
    palette = img;
}
