#ifndef ELEMENTS_BEHAVIOUR_BEHAVIOUR_BASE
#define ELEMENTS_BEHAVIOUR_BEHAVIOUR_BASE

#include "Cell.hpp"
#include "Elements/ElementProperties.hpp"
#include "WorldDelegate.hpp"
#include <SFML/System/Vector2.hpp>

class BehaviourBase {
public:
    virtual bool ActOnSelf  (sf::Vector2i p, CellState &cell, ConstProperties &constProp, WorldDelegate &delegate) { return false; }
    virtual bool ActOnOther (sf::Vector2i p, CellState &cell, ConstProperties &constProp, WorldDelegate &delegate) { return false; }

    virtual void ReceiveHeat(CellState &cell)       { return; }
    virtual void ReceiveExplosion(CellState &cell)  { return; }
};

#endif