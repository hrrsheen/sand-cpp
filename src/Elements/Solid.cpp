#include "Actions.hpp"
#include "Elements/Names.hpp"
#include "Elements/ElementProperties.hpp"
#include "Elements/Solid.hpp"
#include "SandWorld.hpp"

//////////////////////////////////////////////////////////////////////////////////////////
//  Sand
//////////////////////////////////////////////////////////////////////////////////////////


Action Sand::ActUponOther(Cell &self,  ElementProperties &selfProp,
                          Cell &other, ElementProperties &otherProp,
                          sf::Vector2i p, sf::Vector2i otherP,
                          float dt) const {
    if (other.id == Element::fire) {
        return Action(otherP, Element::air);
    }

    return Action::Null();
}
