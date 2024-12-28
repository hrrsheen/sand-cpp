#include "Elements/ElementProperties.hpp"
#include "Elements/Gas.hpp"
#include "SandWorld.hpp"

Gas::Gas() : ElementProperties(ElementType::GAS) {}
Gas::Gas(Element thisId, std::string_view thisName, MoveType move, uint8_t spread) : 
    ElementProperties(thisId, ElementType::GAS, thisName, move, spread) {}

//////////////////////////////////////////////////////////////////////////////////////////
//  Fire
//////////////////////////////////////////////////////////////////////////////////////////

Fire::Fire() : Gas(Element::fire, "fire", MoveType::FLOAT_UP, SpreadType::SIDE | SpreadType::UP_SIDE) {
    std::get<COLOUR_INDEX>(palette).push_back(0xff3b14ff);
    std::get<COLOUR_INDEX>(palette).push_back(0xff7429ff);
    std::get<COLOUR_INDEX>(palette).push_back(0xf59d18ff);
    std::get<COLOUR_INDEX>(palette).push_back(0xfcaa2dff);
    std::get<COLOUR_INDEX>(palette).push_back(0xff3d24ff);
    std::get<COLOUR_INDEX>(palette).push_back(0xff983dff);
}

bool Fire::ActUponSelf(sf::Vector2i p, Cell &self, SandWorld &world, float dt) {
    // Tick down the fire's own health.
    if (self.health <= 0) {
        if (QuickRandInt(100) > 89) {
            world.Act(world.ToIndex(p.x, p.y), -1, Element::smoke, Element::null);
        } else {
            world.Act(world.ToIndex(p.x, p.y), -1, Element::air, Element::null);
        }
        return true;
    }

    self.health -= (500.f + static_cast<float>(QuickRandRange(-100, 100))) * dt;
    return false;
}

bool Fire::ActUponNeighbours(sf::Vector2i p, SandWorld &world) {
    return false;
}

bool Fire::ActUpon(sf::Vector2i p, sf::Vector2i target, Cell &cell, Solid &properties, SandWorld &world) {
    return false;
}

bool Fire::ActUpon(sf::Vector2i p, sf::Vector2i target, Cell &cell, Gas   &properties, SandWorld &world) {
    return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Smoke
//////////////////////////////////////////////////////////////////////////////////////////

Smoke::Smoke() : Gas(Element::smoke, "smoke", MoveType::FLOAT_UP, SpreadType::SIDE | SpreadType::UP_SIDE) {
    std::get<COLOUR_INDEX>(palette).push_back(0xbdbdbdff);
    std::get<COLOUR_INDEX>(palette).push_back(0x616161ff);
    std::get<COLOUR_INDEX>(palette).push_back(0xd1cfcfff);
    std::get<COLOUR_INDEX>(palette).push_back(0xb3b3b3ff);
}

bool Smoke::ActUponSelf(sf::Vector2i p, Cell &self, SandWorld &world, float dt) {
    // Tick down the smoke's own health.
    if (self.health <= 0) {
        world.Act(world.ToIndex(p.x, p.y), -1, Element::air, Element::null);
        return true;
    }

    self.health -= (100.f + static_cast<float>(QuickRandRange(-50, 50))) * dt;
    return false;
}
