#include "Helpers.hpp"
#include "SandWorker.hpp"
#include "WorldState.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <iostream>
#include <vector>

WorldState::WorldState(int width, int height) : world(-2, 2, 0, 2) {}

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
        WorldDisplay &display {world.display[id]};
    
        for (int ci = 0; ci < room.chunks.Size(); ci++) {
            Chunk &chunk {room.chunks.GetChunk(ci)};
            for (int y = chunk.yMin; y < chunk.yMax; ++y) {
                for (int x = chunk.xMin; x < chunk.xMax; ++x) {
                    Cell &cell {room.GetCell(x, y)};
                    if (cell.redraw) {
                        display.gridImage.setPixel(x, y, cell.colour);
                        cell.redraw = false;
                    }
                }
            }
        }
        display.gridTexture.loadFromImage(display.gridImage);
        display.gridSprite.setTexture(display.gridTexture);
        screen.Draw(display.gridSprite);
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