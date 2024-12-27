#include "Elements/ElementProperties.hpp"
#include "Elements/Solid.hpp"
#include "SandWorld.hpp"

Solid::Solid() : ElementProperties(ElementType::SOLID) {}
Solid::Solid(Element thisId, std::string_view thisName, MoveType move, uint8_t spread) : 
    ElementProperties(thisId, ElementType::SOLID, thisName, move, spread) {}

//////////////////////////////////////////////////////////////////////////////////////////
//  Sand
//////////////////////////////////////////////////////////////////////////////////////////

Sand::Sand() : Solid(Element::sand, "sand", MoveType::ACCELERATE_DOWN, SpreadType::DOWN_SIDE) {
    std::get<COLOUR_INDEX>(palette).push_back(0xfabf73ff);
    std::get<COLOUR_INDEX>(palette).push_back(0xebae60ff);
}

bool Sand::ActUponNeighbours(sf::Vector2i p, SandWorld &world) {
    // Put out fires?
    return false;
}

bool Sand::ActUpon(sf::Vector2i p, sf::Vector2i target, Cell &cell, Liquid &properties, SandWorld &world) {
    return false;
}

bool Sand::CanDisplace(ElementProperties &other) const {
    if (other.type == ElementType::SOLID) {
        return false;
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Stone
//////////////////////////////////////////////////////////////////////////////////////////

Stone::Stone() : Solid(Element::stone, "stone", MoveType::NONE, SpreadType::NONE) {
    sf::Image img;
    img.loadFromFile("./assets/stone-texture.png");
    palette = img;
}
