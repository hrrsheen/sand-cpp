#include "Elements/Names.hpp"
#include "Elements/ElementProperties.hpp"
#include "Elements/Gas.hpp"
#include "SandWorld.hpp"

//////////////////////////////////////////////////////////////////////////////////////////
//  Fire
//////////////////////////////////////////////////////////////////////////////////////////

Action Fire::ActUponSelf(sf::Vector2i p, Cell &self, float dt) const {
    // Tick down the fire's own health.
    if (self.health <= 0) {
        if (Probability(20))    return Action(p, Element::smoke);
        else                    return Action(p, Element::air);
    }

    self.health -= (500.f + static_cast<float>(QuickRandInt(200))) * dt;
    return Action::Null();
}

Action Fire::ActUponOther(Cell &self,  ElementProperties &selfProp,
                          Cell &other, ElementProperties &otherProp,
                          sf::Vector2i p, sf::Vector2i otherP,
                          float dt) const {
    if (otherProp.flammability > 0.f) {
        self.health += flammability * dt;
        if (other.health <= 0.f) {
            return Action(otherP, Element::fire); // TODO: Will need to change the data passed to allow other p into Action.
        } else {
            other.health -= flammability * dt;
        }
    }

    return Action::Null();
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Smoke
//////////////////////////////////////////////////////////////////////////////////////////

Action Smoke::ActUponSelf(sf::Vector2i p, Cell &self, float dt) const {
    // Tick down the smoke's own health.
    if (self.health <= 0) {
        return Action(p, Element::air);
    }

    self.health -= (100.f + static_cast<float>(QuickRandRange(-50, 50))) * dt;
    return Action::Null();
}
