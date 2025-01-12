#include "Elements/ElementProperties.hpp"
#include "Elements/Liquid.hpp"
#include "SandWorld.hpp"

//////////////////////////////////////////////////////////////////////////////////////////
//  Water
//////////////////////////////////////////////////////////////////////////////////////////

Water InitWater() {
    ConstProperties init;
    init.name               = "water";
    init.type               = ElementType::LIQUID;
    init.moveBehaviour      = MoveType::FLOAT_DOWN;
    init.spreadBehaviour    = SpreadType::DOWN_SIDE | SpreadType::SIDE;
    init.actionSet          = moveset_t {sf::Vector2i(0, -1)};
    COLOUR(init.palette).push_back(0x347debff);

    Water water(init);

    return water;
}

Water::Water(ConstProperties &init) : ElementProperties(Element::water, init) {}

Action Water::ActUponOther(Cell &self,  ElementProperties &selfProp,
                          Cell &other, ElementProperties &otherProp,
                          sf::Vector2i deltaP, float dt) const {
    if (other.id == Element::fire) {
        Action(deltaP, Element::smoke);
    }

    return Action::Null();
}