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
    bool ActUponNeighbours(sf::Vector2i p, SandWorld &world);
    bool ActUpon(sf::Vector2i p, sf::Vector2i target, Cell &cell, Solid &properties, SandWorld &world);
    bool ActUpon(sf::Vector2i p, sf::Vector2i target, Cell &cell, Gas   &properties, SandWorld &world);

    //////// Virtual constants. ////////
    
};

#endif