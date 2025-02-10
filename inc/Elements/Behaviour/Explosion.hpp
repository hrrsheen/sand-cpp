#ifndef ELEMENTS_BEHAVIOUR_EXPLOSION
#define ELEMENTS_BEHAVIOUR_EXPLOSION

#include "Elements/Behaviour/BehaviourBase.hpp"

class Explosion : public BehaviourBase {
    bool ActOnSelf  (sf::Vector2i p, CellState &cell, ConstProperties &constProp, WorldDelegate &delegate);
    bool ActOnOther (sf::Vector2i p, CellState &cell, ConstProperties &constProp, WorldDelegate &delegate);
};

#endif