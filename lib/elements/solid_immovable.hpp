#ifndef SOLID_IMMOVABLE_HPP
#define SOLID_IMMOVABLE_HPP

#include "elements/element_properties.hpp"

class SolidImmovable : public ElementProperties {
public:
    SolidImmovable();
    SolidImmovable(Element thisId, std::string_view thisName);
};

class Stone : public SolidImmovable {
public:
    Stone();
};

#endif