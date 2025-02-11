#ifndef ELEMENTS_BEHAVIOUR_GAS
#define ELEMENTS_BEHAVIOUR_GAS

#include <SFML/System/Vector2.hpp>

class CellState;
class ConstProperties;
class WorldDelegate;

bool ExplosionActOnSelf  (sf::Vector2i p, CellState &cell, ConstProperties &constProp, WorldDelegate &delegate);
bool ExplosionActOnOther (sf::Vector2i p, CellState &cell, ConstProperties &constProp, WorldDelegate &delegate);

#endif