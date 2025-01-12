#include "Actions.hpp"
#include "Elements/Names.hpp"
#include "Elements/ElementProperties.hpp"
#include "Elements/Solid.hpp"
#include "SandWorld.hpp"

//////////////////////////////////////////////////////////////////////////////////////////
//  Sand
//////////////////////////////////////////////////////////////////////////////////////////

Sand InitSand() {
    ConstProperties init;
    init.name               = "sand";
    init.type               = ElementType::SOLID;
    init.moveBehaviour      = MoveType::FALL_DOWN;
    init.spreadBehaviour    = SpreadType::DOWN_SIDE;
    init.actionSet          = moveset_t {sf::Vector2i(0, -1)};
    init.colourEachFrame    = false;
    COLOUR(init.palette).push_back(0xfabf73ff);
    COLOUR(init.palette).push_back(0xebae60ff);

    Sand sand(init);

    return sand;
}


Sand::Sand(ConstProperties &init) : ElementProperties(Element::sand, init) {}

Action Sand::ActUponOther(Cell &self,  ElementProperties &selfProp,
                          Cell &other, ElementProperties &otherProp,
                          sf::Vector2i deltaP, float dt) const {
    if (other.id == Element::fire) {
        return Action(deltaP, Element::air);
    }

    return Action::Null();
}

bool Sand::CanDisplace(ElementType other) const {
    if (other == ElementType::SOLID) {
        return false;
    }

    return true;
}

inline const moveset_t SandActionset() {
    return moveset_t {sf::Vector2i {0, -1}};
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Stone
//////////////////////////////////////////////////////////////////////////////////////////

Stone InitStone() {
    sf::Image img;
    img.loadFromFile("./assets/stone2-texture.png");

    ConstProperties init;
    init.name               = "stone";
    init.type               = ElementType::SOLID;
    init.colourEachFrame    = false;
    init.palette            = img;

    Stone stone(init);

    return stone;
}

Stone::Stone(ConstProperties &init) : ElementProperties(Element::stone, init) {
    
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Wood
//////////////////////////////////////////////////////////////////////////////////////////

Wood InitWood() {
    sf::Image img;
    img.loadFromFile("./assets/wood-texture.png");

    ConstProperties init;
    init.name               = "wood";
    init.type               = ElementType::SOLID;
    init.colourEachFrame    = false;
    init.palette            = img;
    init.flammability       = 100.f;

    Wood wood(init);

    return wood;
}

Wood::Wood(ConstProperties &init) : ElementProperties(Element::wood, init) {}
