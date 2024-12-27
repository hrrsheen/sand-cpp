#ifndef GAS_HPP
#define GAS_HPP

#include "Elements/ElementProperties.hpp"

class Gas : public ElementProperties {
public:
    Gas();
    Gas(Element thisId, std::string_view thisName, MoveType move, uint8_t spread);
};

#endif