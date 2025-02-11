#include "Cell.hpp"
#include "Elements/ElementProperties.hpp"
#include "Elements/ElementBehaviours.hpp"
#include "Elements/Behaviour/Gas.hpp"
#include "WorldDelegate.hpp"

bool ActOnSelf  (sf::Vector2i p, CellState &cell, ConstProperties &constProp, WorldDelegate &delegate) {
    switch(cell.id) {
        case Element::explosion:
            return ExplosionActOnSelf(p, cell, constProp, delegate);
        default:
            return false;
    }
}

bool ActOnOther (sf::Vector2i p, CellState &cell, ConstProperties &constProp, WorldDelegate &delegate) {
    switch(cell.id) {
        case Element::explosion:
            return ExplosionActOnOther(p, cell, constProp, delegate);
        default:
            return false;
    }
}
