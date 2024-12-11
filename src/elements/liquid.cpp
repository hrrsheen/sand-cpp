#include "elements/element_properties.hpp"
#include "elements/liquid.hpp"

Liquid::Liquid() : ElementProperties(ElementType::liquid) {}
Liquid::Liquid(Element thisId, std::string_view thisName) : 
    ElementProperties(thisId, ElementType::liquid, thisName) {}

bool Liquid::canDisplace(ElementProperties &other) {
    if (other.type == ElementType::solidImmovable) {
        return false;
    }

    if (other.type == ElementType::air || other.type == ElementType::gas) {
        return true;
    }

    return density() > other.density();
}

Water::Water() : Liquid(Element::water, "water") {
    std::get<ElementProperties::colourIndex>(palette).push_back(0x347debff);
}