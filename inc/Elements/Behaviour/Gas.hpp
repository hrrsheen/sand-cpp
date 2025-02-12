#ifndef ELEMENTS_BEHAVIOUR_GAS
#define ELEMENTS_BEHAVIOUR_GAS

#include <SFML/System/Vector2.hpp>

class CellState;
class ConstProperties;
class WorldDelegate;

bool FireActOnSelf      (sf::Vector2i p, CellState &cell, ConstProperties &constProp, WorldDelegate &delegate);
bool FireActOnOther     (sf::Vector2i p, CellState &cell, ConstProperties &constProp, WorldDelegate &delegate);

bool SmokeActOnSelf     (sf::Vector2i p, CellState &cell, ConstProperties &constProp, WorldDelegate &delegate);

bool SparkActOnSelf     (sf::Vector2i p, CellState &cell, ConstProperties &constProp, WorldDelegate &delegate);

bool ExplosionActOnSelf (sf::Vector2i p, CellState &cell, ConstProperties &constProp, WorldDelegate &delegate);
bool ExplosionActOnOther(sf::Vector2i p, CellState &cell, ConstProperties &constProp, WorldDelegate &delegate);

#endif