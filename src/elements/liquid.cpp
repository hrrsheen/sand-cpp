#include "Elements/ElementProperties.hpp"
#include "Elements/Liquid.hpp"

Liquid::Liquid() : ElementProperties(ElementType::LIQUID) {}
Liquid::Liquid(Element thisId, std::string_view thisName, MoveType move, uint8_t spread) : 
    ElementProperties(thisId, ElementType::LIQUID, thisName, move, spread) {}

bool Liquid::CanDisplace(ElementProperties &other) const {
    if (other.type == ElementType::AIR || other.type == ElementType::GAS) {
        return true;
    }

    return false;
}

Water::Water() : Liquid(Element::water, "water", MoveType::FLOAT_DOWN, SpreadType::DOWN_SIDE | SpreadType::SIDE) {
    std::get<COLOUR_INDEX>(palette).push_back(0x347debff);
}