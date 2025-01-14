#include "Elements/Inits.hpp"

//////////////////////////////////////////////////////////////////////////////////////////
//  Solids.
//////////////////////////////////////////////////////////////////////////////////////////

bool InitSand(ElementProperties &properties) {
    ConstProperties constsInit;
    constsInit.name               = "sand";
    constsInit.type               = ElementType::SOLID;
    constsInit.moveBehaviour      = MoveType::FALL_DOWN;
    constsInit.spreadBehaviour    = SpreadType::DOWN_SIDE;
    constsInit.actionSet          = moveset_t {sf::Vector2i(0, -1)};
    ColourProperties colourInit;
    colourInit.colourEachFrame    = false;
    COLOUR(colourInit.palette).push_back(0xfabf73ff);
    COLOUR(colourInit.palette).push_back(0xebae60ff);

    return properties.Insert(Element::sand, constsInit, colourInit);
}

bool InitStone(ElementProperties &properties) {
    ConstProperties constsInit;
    constsInit.name     = "stone";
    constsInit.type     = ElementType::SOLID;
    ColourProperties colourInit;
    sf::Image img;
    img.loadFromFile("./assets/stone2-texture.png");
    colourInit.palette  = img;

    return properties.Insert(Element::stone, constsInit, colourInit);
}

bool InitWood(ElementProperties &properties) {
    ConstProperties constsInit;
    constsInit.name         = "wood";
    constsInit.type         = ElementType::SOLID;
    constsInit.flammability = 100.f;
    ColourProperties colourInit;
    sf::Image img;
    img.loadFromFile("./assets/wood-texture.png");
    colourInit.palette      = img;

    return properties.Insert(Element::wood, constsInit, colourInit);
}


//////////////////////////////////////////////////////////////////////////////////////////
//  Liquids.
//////////////////////////////////////////////////////////////////////////////////////////

bool InitWater(ElementProperties &properties) {
    ConstProperties constsInit;
    constsInit.name               = "water";
    constsInit.type               = ElementType::LIQUID;
    constsInit.moveBehaviour      = MoveType::FLOAT_DOWN;
    constsInit.spreadBehaviour    = SpreadType::DOWN_SIDE | SpreadType::SIDE;
    constsInit.actionSet          = moveset_t {sf::Vector2i(0, -1)};
    ColourProperties colourInit;
    COLOUR(colourInit.palette).push_back(0x347debff);

    return properties.Insert(Element::water, constsInit, colourInit);
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Gasses.
//////////////////////////////////////////////////////////////////////////////////////////

inline const moveset_t FireActionset() {
    return moveset_t {
        sf::Vector2i {-1,  1}, sf::Vector2i {0,  1}, sf::Vector2i {1,  1},
        sf::Vector2i {-1,  0},                       sf::Vector2i {1,  0},
        sf::Vector2i {-1, -1}, sf::Vector2i {0, -1}, sf::Vector2i {1, -1}
    };
}

bool InitFire(ElementProperties &properties) {
    ConstProperties constsInit;
    constsInit.name               = "fire";
    constsInit.type               = ElementType::GAS;
    constsInit.moveBehaviour      = MoveType::FLOAT_UP;
    constsInit.spreadBehaviour    = SpreadType::SIDE | SpreadType::UP_SIDE;
    constsInit.actionSet          = FireActionset();
    ColourProperties colourInit;
    colourInit.colourEachFrame    = true;
    COLOUR(colourInit.palette).push_back(0xff3b14ff);
    COLOUR(colourInit.palette).push_back(0xff7429ff);
    COLOUR(colourInit.palette).push_back(0xf59d18ff);
    COLOUR(colourInit.palette).push_back(0xfcaa2dff);
    COLOUR(colourInit.palette).push_back(0xff3d24ff);
    COLOUR(colourInit.palette).push_back(0xff983dff);

    return properties.Insert(Element::fire, constsInit, colourInit);
}

bool InitSmoke(ElementProperties &properties) {
    ConstProperties constsInit;
    constsInit.name               = "smoke";
    constsInit.type               = ElementType::GAS;
    constsInit.moveBehaviour      = MoveType::FLOAT_UP;
    constsInit.spreadBehaviour    = SpreadType::SIDE | SpreadType::UP_SIDE;
    ColourProperties colourInit;
    COLOUR(colourInit.palette).push_back(0xbdbdbdff);
    COLOUR(colourInit.palette).push_back(0x616161ff);
    COLOUR(colourInit.palette).push_back(0xd1cfcfff);
    COLOUR(colourInit.palette).push_back(0xb3b3b3ff);

    return properties.Insert(Element::smoke, constsInit, colourInit);
}