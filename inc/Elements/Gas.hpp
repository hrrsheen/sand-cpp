#ifndef GAS_HPP
#define GAS_HPP

#include "Elements/ElementProperties.hpp"

class Fire : public ElementProperties {
public:
    Fire(ConstProperties &init);

    //////// Simulation functions. ////////
    Action ActUponSelf  (sf::Vector2i p, Cell &self, float dt) const;
    Action ActUponOther (Cell &self,  ElementProperties &selfProp,
                         Cell &other, ElementProperties &otherProp,
                         sf::Vector2i p, sf::Vector2i otherP,
                         float dt) const;
    
};

const moveset_t FireActionset();

class Smoke : public ElementProperties {
public:
    Smoke(ConstProperties &init);

    //////// Simulation functions. ////////
    Action ActUponSelf(sf::Vector2i p, Cell &self, float dt);
    
};

Fire    InitFire();
Smoke   InitSmoke();

#endif