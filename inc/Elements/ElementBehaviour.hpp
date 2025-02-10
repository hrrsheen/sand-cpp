#ifndef ELEMENTS_ELEMENT_BEHAVIOUR_HPP
#define ELEMENTS_ELEMENT_BEHAVIOUR_HPP

#include "Elements/Behaviour/BehaviourBase.hpp"
#include "Elements/Names.hpp"

using behaviour_ptr = std::unique_ptr<BehaviourBase>;

class ElementBehaviour {
private:
    std::vector<behaviour_ptr> behaviours;

public:
    ElementBehaviour() : behaviours(Element::count + 1) {}

    void Insert(Element id, behaviour_ptr behaviour);

    BehaviourBase* operator[](size_t n);
};

#endif