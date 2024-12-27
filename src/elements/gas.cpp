#include "Elements/ElementProperties.hpp"
#include "Elements/Gas.hpp"

Gas::Gas() : ElementProperties(ElementType::GAS) {}
Gas::Gas(Element thisId, std::string_view thisName, MoveType move, uint8_t spread) : 
    ElementProperties(thisId, ElementType::GAS, thisName, move, spread) {}