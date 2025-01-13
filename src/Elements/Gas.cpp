#include "Elements/Names.hpp"
#include "Elements/ElementProperties.hpp"
#include "Elements/Gas.hpp"
#include "SandWorld.hpp"

//////////////////////////////////////////////////////////////////////////////////////////
//  Fire
//////////////////////////////////////////////////////////////////////////////////////////

Fire InitFire() {
    ConstProperties init;
    init.name               = "fire";
    init.type               = ElementType::GAS;
    init.moveBehaviour      = MoveType::FLOAT_UP;
    init.spreadBehaviour    = SpreadType::SIDE | SpreadType::UP_SIDE;
    init.actionSet          = FireActionset();
    init.colourEachFrame    = true;
    COLOUR(init.palette).push_back(0xff3b14ff);
    COLOUR(init.palette).push_back(0xff7429ff);
    COLOUR(init.palette).push_back(0xf59d18ff);
    COLOUR(init.palette).push_back(0xfcaa2dff);
    COLOUR(init.palette).push_back(0xff3d24ff);
    COLOUR(init.palette).push_back(0xff983dff);

    Fire fire(init);

    return fire;
}

Fire::Fire(ConstProperties &init) : ElementProperties(Element::fire, init) {}

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

inline const moveset_t FireActionset() {
    return moveset_t {
        sf::Vector2i {-1,  1}, sf::Vector2i {0,  1}, sf::Vector2i {1,  1},
        sf::Vector2i {-1,  0},                       sf::Vector2i {1,  0},
        sf::Vector2i {-1, -1}, sf::Vector2i {0, -1}, sf::Vector2i {1, -1}
    };
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Smoke
//////////////////////////////////////////////////////////////////////////////////////////

Smoke InitSmoke() {
    ConstProperties init;
    init.name               = "smoke";
    init.type               = ElementType::GAS;
    init.moveBehaviour      = MoveType::FLOAT_UP;
    init.spreadBehaviour    = SpreadType::SIDE | SpreadType::UP_SIDE;
    COLOUR(init.palette).push_back(0xbdbdbdff);
    COLOUR(init.palette).push_back(0x616161ff);
    COLOUR(init.palette).push_back(0xd1cfcfff);
    COLOUR(init.palette).push_back(0xb3b3b3ff);

    Smoke smoke(init);

    return smoke;
}

Smoke::Smoke(ConstProperties &init) : ElementProperties(Element::smoke, init) {}

Action Smoke::ActUponSelf(sf::Vector2i p, Cell &self, float dt) {
    // Tick down the smoke's own health.
    if (self.health <= 0) {
        return Action(p, Element::air);
    }

    self.health -= (100.f + static_cast<float>(QuickRandRange(-50, 50))) * dt;
    return Action::Null();
}
