#ifndef ACTIONS_HPP
#define ACTIONS_HPP

#include "Elements/Names.hpp"
#include "SandWorld.hpp"
#include <SFML/System/Vector2.hpp>

struct Move {
    roomID_t srcRoomID;
    int src;
    int dst;

    Move() : srcRoomID(-1), src(-1), dst(-1) {}
    Move(roomID_t _srcRoomID, int _src, int _dst) : srcRoomID(_srcRoomID), src(_src), dst(_dst) {}
};

struct Action {
    sf::Vector2i p;
    Element transform;

    Action(sf::Vector2i _p, Element _transform) : p(_p), transform(_transform) {}
    static const Action Null() { return Action(sf::Vector2i{}, Element::null); }
    
    bool IsValid() const { return transform != Element::null; }
};

#endif