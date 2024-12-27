#include "Elements/ElementProperties.hpp"
#include "Elements/Gas.hpp"

Gas::Gas() : ElementProperties(ElementType::GAS) {}
Gas::Gas(Element thisId, std::string_view thisName, MoveType move, uint8_t spread) : 
    ElementProperties(thisId, ElementType::GAS, thisName, move, spread) {}

//////////////////////////////////////////////////////////////////////////////////////////
//  Fire
//////////////////////////////////////////////////////////////////////////////////////////

Fire::Fire() : Gas(Element::fire, "fire", MoveType::FLOAT_UP, SpreadType::SIDE | SpreadType::UP_SIDE) {
    std::get<COLOUR_INDEX>(palette).push_back(0xff3b14ff);
    std::get<COLOUR_INDEX>(palette).push_back(0xff7429ff);
    std::get<COLOUR_INDEX>(palette).push_back(0xf59d18ff);
    std::get<COLOUR_INDEX>(palette).push_back(0xfcaa2dff);
    std::get<COLOUR_INDEX>(palette).push_back(0xff3d24ff);
    std::get<COLOUR_INDEX>(palette).push_back(0xff983dff);
}

bool Fire::ActUponNeighbours(sf::Vector2i p, SandWorld &world) {
    return false;
}

bool Fire::ActUpon(sf::Vector2i p, sf::Vector2i target, Cell &cell, Solid &properties, SandWorld &world) {
    return false;
}

bool Fire::ActUpon(sf::Vector2i p, sf::Vector2i target, Cell &cell, Gas   &properties, SandWorld &world) {
    return false;
}
