#include "Cell.hpp"
#include "Elements/ElementProperties.hpp"
#include "Elements/ElementBehaviours.hpp"
#include "Elements/Behaviour/Gas.hpp"
#include "WorldDelegate.hpp"

bool ActOnSelf  (sf::Vector2i p, CellState &cell, ConstProperties &constProp, WorldDelegate &delegate) {
    switch(cell.id) {
        case Element::fire:
            return FireActOnSelf(p, cell, constProp, delegate);
        case Element::smoke:
            return SmokeActOnSelf(p, cell, constProp, delegate);
        case Element::spark:
            return SparkActOnSelf(p, cell, constProp, delegate);
        case Element::explosion:
            return ExplosionActOnSelf(p, cell, constProp, delegate);
        default:
            return false;
    }
}

bool ActOnOther (sf::Vector2i p, CellState &cell, ConstProperties &constProp, WorldDelegate &delegate) {
    switch(cell.id) {
        case Element::fire:
            return FireActOnOther(p, cell, constProp, delegate);
        case Element::explosion:
            return ExplosionActOnOther(p, cell, constProp, delegate);
        default:
            return false;
    }
}
