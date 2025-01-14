#include "Elements/ElementProperties.hpp"
#include "Elements/Liquid.hpp"
#include "SandWorld.hpp"

//////////////////////////////////////////////////////////////////////////////////////////
//  Water
//////////////////////////////////////////////////////////////////////////////////////////

Action Water::ActUponOther(Cell &self,  ElementProperties &selfProp,
                          Cell &other, ElementProperties &otherProp,
                          sf::Vector2i p, sf::Vector2i otherP,
                          float dt) const {
    if (other.id == Element::fire) {
        return Action(otherP, Element::smoke);
    }

    return Action::Null();
}