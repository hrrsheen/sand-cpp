#ifndef SOLID_HPP
#define SOLID_HPP

#include "Elements/ElementProperties.hpp"

class Sand : public ElementProperties {
public:
    Sand(ConstProperties &init);

    //////// Simulation functions. ////////
    Action ActUponOther(Cell &self,  ElementProperties &selfProp,
                        Cell &other, ElementProperties &otherProp,
                        sf::Vector2i p, sf::Vector2i otherP,
                        float dt) const;

    bool CanDisplace(ElementType other) const;
};

class Stone : public ElementProperties {
public:
    Stone(ConstProperties &init);
};

class Wood : public ElementProperties {
public:
    Wood(ConstProperties &init);
};


Sand    InitSand();
Stone   InitStone();
Wood    InitWood();

#endif