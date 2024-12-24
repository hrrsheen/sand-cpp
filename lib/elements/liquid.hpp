#ifndef LIQUID_HPP
#define LIQUID_HPP

#include "Elements/ElementProperties.hpp"

class Liquid : public ElementProperties {
public:
    Liquid();
    Liquid(Element thisId, std::string_view thisName, MoveType move, uint8_t spread);
};

class Water : public Liquid {
public:
    Water();
};

#endif