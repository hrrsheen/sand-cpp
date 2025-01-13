#include "Helpers.hpp"
#include "SandWorker.hpp"
#include "WorldState.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <iostream>
#include <vector>

WorldState::WorldState(int width, int height) : 
    world(-2, 2, 0, 2) {
    gridImage.create(width, height);
    gridTexture.create(width, height);
    gridTexture.setSmooth(false);
    InitGridImage(width, height);
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
    const sf::Rect<int> borders {screen.ViewBorders()};
    sf::Vector2i size {borders.getSize() - sf::Vector2i(1, 1)};
    std::vector<roomID_t> rooms {
        world.ContainingRoomID(borders.getPosition() - sf::Vector2( size.x,  size.y) / 2),
        world.ContainingRoomID(borders.getPosition() - sf::Vector2(-size.x,  size.y) / 2),
        world.ContainingRoomID(borders.getPosition() - sf::Vector2( size.x, -size.y) / 2),
        world.ContainingRoomID(borders.getPosition() - sf::Vector2(-size.x, -size.y) / 2)
    };
    std::sort(rooms.begin(), rooms.end());
    rooms.erase(std::unique(rooms.begin(), rooms.end()), rooms.end());
    for (roomID_t id : rooms) {
        SandRoom &room {world.GetRoom(id)};

    
        for (int ci = 0; ci < room.chunks.Size(); ci++) {
            Chunk &chunk {room.chunks.GetChunk(ci)};
            if (room.chunks.IsActive(ci)) {
                for (int y = chunk.yMin; y < chunk.yMax; ++y) {
                    for (int x = chunk.xMin; x < chunk.xMax; ++x) {
                        Cell &cell {room.GetCell(x, y)};
                        if (cell.redraw) {
                            UpdateCell(x, y, cell.colour);
                            cell.redraw = false;
                        }
                    }
                }
            }
        }
    }
    // for (int i = 0; i < world.Size() - 1; i++) {
    //     Cell &cell {world.GetCell(i)};
    //     if (cell.redraw) {
    //         sf::Vector2i coords {world.ToCoords(i)};
    //         UpdateCell(coords.x, coords.y, cell.Colour());
    //         cell.redraw = false;
    //     }
    // }
    DrawWorld(screen);
}

void WorldState::InitGridImage(int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            gridImage.setPixel(x, y, sf::Color::Black);
        }
    }
}

void WorldState::UpdateCell(int x, int y, sf::Color colour) {
    gridImage.setPixel(x, y, colour);
}

void WorldState::DrawWorld(Screen &screen) {
    gridTexture.loadFromImage(gridImage);
    gridSprite.setTexture(gridTexture);
    screen.Draw(gridSprite);
}