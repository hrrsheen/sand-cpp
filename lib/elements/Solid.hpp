#ifndef SOLID_HPP
#define SOLID_HPP

#include "Elements/ElementProperties.hpp"

class Solid : public ElementProperties {
public:
    Solid();
    Solid(Element thisId, std::string_view thisName, MoveType move, uint8_t spread);
};

class Sand : public Solid {
public:
    Sand();

    //////// Simulation functions. ////////
    bool ActUponNeighbours(sf::Vector2i p, SandWorld &world);

    bool CanDisplace(ElementProperties &other) const;
};

class Stone : public Solid {
public:
    Stone();
};

#endif