#include "elements/element_properties.hpp"
#include "elements/solid_movable.hpp"

SolidMovable::SolidMovable() : ElementProperties(ElementType::solidMovable) {}
SolidMovable::SolidMovable(Element thisId, std::string_view thisName) : 
    ElementProperties(thisId, ElementType::solidMovable, thisName) {}

bool SolidMovable::canDisplace(ElementProperties &other) {
    if (other.type == ElementType::solidMovable || other.type == ElementType::solidImmovable) {
        return false;
    }

    if (other.type == ElementType::air || other.type == ElementType::gas) {
        return true;
    }

    return density() > other.density();
}


Sand::Sand() : SolidMovable(Element::sand, "sand") {
    std::get<ElementProperties::colourIndex>(palette).push_back(0xfabf73ff);
    std::get<ElementProperties::colourIndex>(palette).push_back(0xebae60ff);
}
