#ifndef SANDWORKER_HPP
#define SANDWORKER_HPP

#include "Cell.hpp"
#include "Elements/Names.hpp"
#include "Elements/ElementProperties.hpp"
#include "Elements/PropertiesContainer.hpp"
#include "SandRoom.hpp"
#include "SandWorld.hpp"
#include <utility>

class SandWorker {
protected:
    roomID_t    thisID;
    SandWorld   &world;
    SandRoom* const room;

    PropertiesContainer &properties;

public:
    SandWorker(roomID_t id, SandWorld &_world, SandRoom *_room, PropertiesContainer &_properties);

private:
    Cell &GetCell(int x, int y);
    Cell &GetCell(sf::Vector2i p);
    void SetCell(int x, int y, Element id);

    roomID_t ContainingRoomID(sf::Vector2i p);
    SandRoom *GetRoom(roomID_t id);

    // Returns the containing room's ID if the given point is empty, -1 otherwise.
    roomID_t IsEmpty(int x, int y);
    roomID_t IsEmpty(sf::Vector2i p);
    std::pair<roomID_t, sf::Vector2i> PathEmpty(sf::Vector2i start, sf::Vector2i end);

    //////// Movement functions ////////
    bool FloatDown      (sf::Vector2i p, Cell &cell);
    bool FloatUp        (sf::Vector2i p, Cell &cell);
    bool FallDown       (sf::Vector2i p, Cell &cell, float dt);

    bool SpreadDownSide (sf::Vector2i p, Cell &cell);
    bool SpreadUpSide   (sf::Vector2i p, Cell &cell);
    bool SpreadSide     (sf::Vector2i p, Cell &cell);
};

#endif