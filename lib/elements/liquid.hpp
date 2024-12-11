#ifndef LIQUID_HPP
#define LIQUID_HPP

#include "elements/element_properties.hpp"

class Liquid : public ElementProperties {
public:
    Liquid();
    Liquid(Element thisId, std::string_view thisName);

    // Simulation constants.
    virtual const float spreadRate() const { return 1.f; }
    const bool isMobile() const { return true; }

    // Simulation functions.
    bool canDisplace(ElementProperties &other);
};

class Water : public Liquid {
public:
    Water();

    // Simulation constants.
    const float spreadRate() const { return 5.f; }
    const float density() const { return 1.f; }
};

#endif