#ifndef ELEMENTS_ELEMENT_BEHAVIOURS_HPP
#define ELEMENTS_ELEMENT_BEHAVIOURS_HPP

#include <SFML/System/Vector2.hpp>

class CellState;
class ConstProperties;
class WorldDelegate;

bool ActOnSelf  (sf::Vector2i p, CellState &cell, ConstProperties &constProp, WorldDelegate &delegate);
bool ActOnOther (sf::Vector2i p, CellState &cell, ConstProperties &constProp, WorldDelegate &delegate);

#endif