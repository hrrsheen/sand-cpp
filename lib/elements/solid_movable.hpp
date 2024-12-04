#ifndef SOLID_MOVABLE_HPP
#define SOLID_MOVABLE_HPP

#include "elements/element_properties.hpp"

class SolidMovable : public ElementProperties {
public:
    SolidMovable();
    SolidMovable(Element thisId, std::string_view thisName);

    // Simulation constants.
    virtual const float spreadRate() const { return 0.f; }
    const bool isMobile() const { return true; }

    // Simulation functions.
    bool canDisplace(ElementProperties &other);
};

class Sand : public SolidMovable {
public:
    Sand();

    // Constants
    const float spreadRate() const { return 1.f; }
    const float density() const { return 1.f; }
};

#endif