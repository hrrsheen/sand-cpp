#include "Helpers.hpp"
#include "SandWorker.hpp"
#include "WorldState.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

WorldState::WorldState(int width, int height) : world(-2, 2, 0, 2) {
    world.SpawnRoom(constants::roomWidth, 0);
    world.SpawnRoom(0, constants::roomHeight);
    gridImage.create(constants::roomWidth, constants::roomHeight);
    gridTexture.create(constants::roomWidth, constants::roomHeight);
    gridTexture.setSmooth(false);
}

void WorldState::Step(float dt) {
    for (roomID_t id = 0; id < world.rooms.Range(); ++id) {
        SandWorker worker {id, world, &world.GetRoom(id)};
        worker.Step(dt);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Draw functions.
//////////////////////////////////////////////////////////////////////////////////////////

void WorldState::Draw(Screen &screen) {
    screen.clear();
    const sf::IntRect borders {screen.ViewBorders()};
    std::vector<std::pair<sf::Vector2i, roomID_t>> rooms {VisibleRooms(borders)};
    gridSprite.setPosition(rooms[0].first.x, rooms[0].first.y);

    int xMin, xMax, // Determines the potion of a room that's drawn.
        yMin, yMax;
    
    std::vector<roomID_t> completed;
    completed.reserve(4);
    for (int i = 0; i < rooms.size(); ++i) {
        if (std::find(completed.begin(), completed.end(), rooms[i].second) != completed.end())
            continue; // Room already processes.
        if (!VALID_ROOM(rooms[i].second))
            continue;

        SandRoom &room {world.GetRoom(rooms[i].second)};
        sf::Vector2i intersect {rooms[i].first};
        // Figure out which portion of the room to draw based on the view corner that intersects it.
        switch (i) {
            case 0:
                xMin = intersect.x; xMax = room.x + room.width;
                yMin = intersect.y; yMax = room.y + room.height;
                break;
            case 1:
                xMin = room.x;      xMax = intersect.x;
                yMin = intersect.y; yMax = room.y + room.height;
                break;
            case 2:
                xMin = intersect.x; xMax = room.x + room.width;
                yMin = room.y;      yMax = intersect.y;
                break;
            case 3:
                xMin = room.x;      xMax = intersect.x;
                yMin = room.y;      yMax = intersect.y;
                break;
        }
        
        // Update pixels for the visible portion of the room.
        for (int y = yMin; y < yMax; ++y) {
        for (int x = xMin; x < xMax; ++x) {
            gridImage.setPixel(x - rooms[0].first.x, y - rooms[0].first.y, 
                                room.grid.colour[room.ToIndex(x, y)]); // Need to convert world coords to view coords
        }
        }
        completed.push_back(rooms[i].second);
    }
    gridTexture.loadFromImage(gridImage);
    gridSprite.setTexture(gridTexture);
    screen.Draw(gridSprite);
}

std::vector<std::pair<sf::Vector2i, roomID_t>> WorldState::VisibleRooms(const sf::IntRect borders) {
    sf::Vector2i size {borders.getSize() - sf::Vector2i(1, 1)};
    std::vector<std::pair<sf::Vector2i, roomID_t>> rooms;
    rooms.reserve(4);
    sf::Vector2i bl {borders.getPosition() - sf::Vector2( size.x,  size.y) / 2};
    sf::Vector2i br {borders.getPosition() - sf::Vector2(-size.x,  size.y) / 2};
    sf::Vector2i tl {borders.getPosition() - sf::Vector2( size.x, -size.y) / 2};
    sf::Vector2i tr {borders.getPosition() - sf::Vector2(-size.x, -size.y) / 2};

    rooms.push_back(std::make_pair(bl, world.ContainingRoomID(bl)));
    rooms.push_back(std::make_pair(br, world.ContainingRoomID(br)));
    rooms.push_back(std::make_pair(tl, world.ContainingRoomID(tl)));
    rooms.push_back(std::make_pair(tr, world.ContainingRoomID(tr)));

    return rooms;
}