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
    bool ActUponNeighbours(sf::Vector2i p, Cell &self, SandWorld &world, float dt);

    bool CanDisplace(ElementProperties &other) const;
};

class Stone : public Solid {
public:
    Stone();
};

class Wood : public Solid {
public:
    Wood();

    virtual const float Flammability() const { return 100.f; }
};

#endif