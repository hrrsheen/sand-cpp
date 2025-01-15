#include "Helpers.hpp"
#include "SandWorker.hpp"
#include "WorldState.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <iostream>
#include <vector>

WorldState::WorldState(int width, int height) : world(-2, 2, 0, 2) {
    roomID_t newID {0};
    display.Insert(WorldDisplay(0, 0));
    newID = world.SpawnRoom(constants::roomWidth, 0);
    display.Insert(WorldDisplay(constants::roomWidth, 0));
    newID = world.SpawnRoom(0, constants::roomHeight);
    display.Insert(WorldDisplay(0, constants::roomHeight));
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
    std::vector<roomID_t> rooms {VisibleRooms(borders)};
    for (roomID_t id : rooms) {
        SandRoom &room {world.GetRoom(id)};
        WorldDisplay &disp {display[id]};
    
        for (int ci = 0; ci < room.chunks.Size(); ci++) {
            Chunk &chunk {room.chunks.GetChunk(ci)};
            for (int y = chunk.yMin; y < chunk.yMax; ++y) { 
            for (int x = chunk.xMin; x < chunk.xMax; ++x) {
                CellDisplay &cellDisp {room.grid.display[room.ToIndex(x, y)]};
                if (cellDisp.redraw) {
                    disp.gridImage.setPixel(
                        x - room.x, y - room.y, 
                        cellDisp.colour);
                    cellDisp.redraw = false;
                }
            }}
        }
        disp.gridTexture.loadFromImage(disp.gridImage);
        disp.gridSprite.setTexture(disp.gridTexture);
        screen.Draw(disp.gridSprite);
    }
}

std::vector<roomID_t> WorldState::VisibleRooms(const sf::IntRect borders) {
    sf::Vector2i size {borders.getSize() - sf::Vector2i(1, 1)};
    std::vector<roomID_t> rooms {
        world.ContainingRoomID(borders.getPosition() - sf::Vector2( size.x,  size.y) / 2),
        world.ContainingRoomID(borders.getPosition() - sf::Vector2(-size.x,  size.y) / 2),
        world.ContainingRoomID(borders.getPosition() - sf::Vector2( size.x, -size.y) / 2),
        world.ContainingRoomID(borders.getPosition() - sf::Vector2(-size.x, -size.y) / 2)
    };
    std::sort(rooms.begin(), rooms.end());
    rooms.erase(std::unique(rooms.begin(), rooms.end()), rooms.end());
    if (!VALID_ROOM(rooms[0])) {
        rooms.erase(rooms.begin());
    }

    return rooms;
}