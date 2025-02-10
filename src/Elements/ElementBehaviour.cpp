#include "Elements/ElementBehaviour.hpp"

void ElementBehaviour::Insert(Element id, behaviour_ptr behaviour) {
    behaviours[id] = std::move(behaviour);
}