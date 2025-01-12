#ifndef LIQUID_HPP
#define LIQUID_HPP

#include "Actions.hpp"
#include "Elements/ElementProperties.hpp"

class Water : public ElementProperties {
public:
    Water(ConstProperties &init);

    //////// Simulation functions. ////////
    Action ActUponOther(Cell &self,  ElementProperties &selfProp,
                        Cell &other, ElementProperties &otherProp,
                        sf::Vector2i deltaP, float dt) const;

    // Virtual constants.
    const int SpreadRate() const { return 7; }
};

Water   InitWater();

#endif