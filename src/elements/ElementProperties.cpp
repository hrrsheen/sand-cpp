#include "Elements.hpp"
#include "Helpers.hpp"
#include "SandWorld.hpp"
#include <cmath>
#include <fstream>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>

ElementProperties::ElementProperties() : 
    id(Element::null), 
    type(),
    name(),
    moveBehaviour(MoveType::NONE), 
    spreadBehaviour(SpreadType::NONE) {
}

ElementProperties::ElementProperties(ElementType thisType) : 
    id(), 
    type(thisType), 
    name(), 
    moveBehaviour(MoveType::NONE), 
    spreadBehaviour(SpreadType::NONE) {}

ElementProperties::ElementProperties(Element thisId, ElementType thisType, std::string_view thisName) :
    id(thisId), type(thisType), name(thisName), moveBehaviour(MoveType::NONE), spreadBehaviour(SpreadType::NONE) {}

ElementProperties::ElementProperties(Element thisId, ElementType thisType, std::string_view thisName, MoveType movement, uint8_t spread) :
    id(thisId), type(thisType), name(thisName), moveBehaviour(movement), spreadBehaviour(spread) {}

sf::Color ElementProperties::ColourFromArray() {
    if (std::get<COLOUR_INDEX>(palette).size() == 0) {
        return sf::Color(0x00000000);
    }
    if (std::get<COLOUR_INDEX>(palette).size() == 1) {
        return sf::Color(std::get<COLOUR_INDEX>(palette).at(0)); // Trivial case where elements are a single colour.
    }
    int position {QuickRandInt(std::get<COLOUR_INDEX>(palette).size())};
    return sf::Color(std::get<COLOUR_INDEX>(palette).at(position));
}

sf::Color ElementProperties::ColourFromTexture(int x, int y) {
    sf::Vector2u size {std::get<TEXTURE_INDEX>(palette).getSize()};
    x = x % static_cast<int>(size.x);
    y = y % static_cast<int>(size.y);
    return std::get<TEXTURE_INDEX>(palette).getPixel(std::abs(x), std::abs(y));
}

bool ElementProperties::HasTexture() {
    return palette.index() == TEXTURE_INDEX;
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Movement.
//////////////////////////////////////////////////////////////////////////////////////////

bool ElementProperties::FloatDown(sf::Vector2i p, Cell &cell, SandWorld &world) {
    sf::Vector2i queryPos(p.x, p.y - 1);
    if (world.IsEmpty(queryPos.x, queryPos.y)) {
        world.MoveCell(world.ToIndex(p.x, p.y), world.ToIndex(queryPos.x, queryPos.y));
        return true;
    }

    return false;
}

bool ElementProperties::FloatUp(sf::Vector2i p, Cell &cell, SandWorld &world) {
    return SpreadUpSide(p, cell, world);
}

bool ElementProperties::AccelerateDown(sf::Vector2i p, Cell &cell, SandWorld &world, float dt) {
    cell.ApplyAcceleration(sf::Vector2f(0.f, -ACCELERATION), dt);
    sf::Vector2i dp {0, static_cast<int>(cell.velocity.y * dt)};
    
    sf::Vector2i dst {p};
    Lerp lerp(p + sf::Vector2i(0, -1), p + dp);
    for (sf::Vector2i check : lerp) {
        if (world.IsEmpty(check.x, check.y)) {
            dst = check;
        } else {
            break;
        }
    }

    if (dst != p) {
        world.MoveCell(world.ToIndex(p.x, p.y), world.ToIndex(dst.x, dst.y));
        return true;
    }

    return false;
}

bool ElementProperties::SpreadDownSide(sf::Vector2i p, Cell &cell, SandWorld &world) {
    sf::Vector2i leftPos    {p + sf::Vector2i( 1, -1)};
    sf::Vector2i rightPos   {p + sf::Vector2i(-1, -1)};

    bool downLeft   {world.IsEmpty( leftPos.x,  leftPos.y) || CanDisplace( leftPos, world)};
    bool downRight  {world.IsEmpty(rightPos.x, rightPos.y) || CanDisplace(rightPos, world)};

    if (downLeft && downRight) {
        downLeft    = QuickRandInt(100) > 49;
        downRight   = !downLeft;
    }

    if (downLeft) { 
        world.MoveCell(world.ToIndex(p.x, p.y), world.ToIndex( leftPos.x,  leftPos.y));
    } else if (downRight) {
        world.MoveCell(world.ToIndex(p.x, p.y), world.ToIndex(rightPos.x, rightPos.y));
    }

    return downLeft || downRight;
}

bool ElementProperties::SpreadUpSide(sf::Vector2i p, Cell &cell, SandWorld &world) {
    sf::Vector2i leftPos    {p + sf::Vector2i( 1, 1)};
    sf::Vector2i rightPos   {p + sf::Vector2i(-1, 1)};

    bool upLeft   {world.IsEmpty( leftPos.x,  leftPos.y)};
    bool upRight  {world.IsEmpty(rightPos.x, rightPos.y)};

    if (upLeft && upRight) {
        upLeft    = QuickRandInt(100) > 49;
        upRight   = !upLeft;
    }

    if (upLeft) { 
        world.MoveCell(world.ToIndex(p.x, p.y), world.ToIndex( leftPos.x,  leftPos.y));
    } else if (upRight) {
        world.MoveCell(world.ToIndex(p.x, p.y), world.ToIndex(rightPos.x, rightPos.y));
    }

    return upLeft || upRight;
}

bool ElementProperties::SpreadSide(sf::Vector2i p, Cell &cell, SandWorld &world) {
    sf::Vector2i lookAhead  {1, 0};

    bool left   {world.IsEmpty(p - lookAhead)};
    bool right  {world.IsEmpty(p + lookAhead)};

    if (left && right) {
        left    = QuickRandInt(100) > 49;
        right   = !left;
    }

    if (left) {
        sf::Vector2i dst {world.PathEmpty(p - lookAhead, p - SpreadRate() * lookAhead)};
        world.MoveCell(world.ToIndex(p.x, p.y), world.ToIndex( dst.x,  dst.y));
    } else if (right) {
        sf::Vector2i dst {world.PathEmpty(p + lookAhead, p + SpreadRate() * lookAhead)};
        world.MoveCell(world.ToIndex(p.x, p.y), world.ToIndex( dst.x,  dst.y));
    }

    return left || right;
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Simulation.
//////////////////////////////////////////////////////////////////////////////////////////

bool ElementProperties::ActUponSelf(sf::Vector2i p, Cell &self, SandWorld &world, float dt) {
    return false;
}

bool ElementProperties::ActUponNeighbours(sf::Vector2i p, SandWorld &world) {
    return false;
}

bool ElementProperties::ActUpon(sf::Vector2i p, sf::Vector2i target, Cell &cell, ElementProperties &properties, SandWorld &world) {
    switch (properties.type) {
        case ElementType::SOLID:
            return ActUpon(p, target, cell, static_cast<Solid&>(properties), world);
            break;
        case ElementType::LIQUID:
            return ActUpon(p, target, cell, static_cast<Liquid&>(properties), world);
            break;
        case ElementType::GAS:
            return ActUpon(p, target, cell, static_cast<Gas&>(properties), world);
            break;
        default:
            return false;
    }
    return false;
}

bool ElementProperties::CanDisplace(ElementProperties &other) const {
    return false;
}

bool ElementProperties::CanDisplace(sf::Vector2i p, SandWorld &world) const {
    if (world.InBounds(p.x, p.y)) {
        return CanDisplace(world.GetProperties(p));
    }

    return false;
}

bool ElementProperties::operator==(const ElementProperties &otherProperty) const {
    return id == otherProperty.id || name == otherProperty.name;
}
