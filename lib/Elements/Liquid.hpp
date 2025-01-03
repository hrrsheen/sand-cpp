#ifndef LIQUID_HPP
#define LIQUID_HPP

#include "Elements/ElementProperties.hpp"

class Liquid : public ElementProperties {
public:
    Liquid();
    Liquid(Element thisId, std::string_view thisName, MoveType move, uint8_t spread);

    bool CanDisplace(ElementProperties &other) const;
};

class Water : public Liquid {
public:
    Water();

    //////// Simulation functions. ////////
    bool ActUponNeighbours(sf::Vector2i p, Cell &self, SandWorld &world, float dt);

    // Virtual constants.
    const int SpreadRate() const { return 7; }
};

#endif