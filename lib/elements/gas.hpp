#ifndef GAS_HPP
#define GAS_HPP

#include "Elements/ElementProperties.hpp"

class Gas : public ElementProperties {
public:
    Gas();
    Gas(Element thisId, std::string_view thisName, MoveType move, uint8_t spread);
};


class Fire : public Gas {
public:
    Fire();

    const bool RecolourEachFrame() const { return true; }

    //////// Simulation functions. ////////
    bool ActUponSelf(sf::Vector2i p, Cell &self, SandWorld &world, float dt);
    bool ActUponNeighbours(sf::Vector2i p, SandWorld &world);

    //////// Virtual constants. ////////
    
};

class Smoke : public Gas {
public:
    Smoke();

    //////// Simulation functions. ////////
    bool ActUponSelf(sf::Vector2i p, Cell &self, SandWorld &world, float dt);

    //////// Virtual constants. ////////
    
};

#endif