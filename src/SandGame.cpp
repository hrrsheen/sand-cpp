#include "SandWorker.hpp"
#include "SandGame.hpp"
#include "Utility/Line.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

bool ShouldClose(sf::Event &event) {
    if (event.type == sf::Event::Closed)
        return true;
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
        return true;

    return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Mouse.
//////////////////////////////////////////////////////////////////////////////////////////

Mouse::Mouse() : state(MouseState::IDLE), radius(1), brush(Element::air), pos(sf::Vector2i(-1, -1)), prevPos(sf::Vector2i(-1, -1)) {}

void Mouse::Reset() {
    state   = MouseState::IDLE;
    pos     = sf::Vector2i(-1, -1);
    prevPos = sf::Vector2i(-1, -1);
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Game.
//////////////////////////////////////////////////////////////////////////////////////////

SandGame::SandGame() : xMinRooms(-2), xMaxRooms(2), 
                       yMinRooms(-1), yMaxRooms(2), 
                       world(-2, 2, -1, 2), 
                       screen{constants::screenWidth, constants::screenHeight, 
                                constants::viewWidth, constants::viewHeight, "Falling Sand"} {
    gridImage.create(constants::roomWidth, constants::roomHeight);
    gridTexture.create(constants::roomWidth, constants::roomHeight);
    gridTexture.setSmooth(false);

    const int viewHeight {constants::viewHeight};
    screen.SetTransform(
        [viewHeight] {
            sf::Transformable transformation;
            transformation.setOrigin(0, viewHeight);    // 1st transform - scale to world height.
            transformation.setScale(1.f, -1.f);         // 2nd transform - flip so that +y is up.
            return transformation.getTransform();
        }()
    );
}

void SandGame::Run() {
    bool DEBUG = false;
    sf::Clock clock;
    Mouse mouse;
    int     fpsElapsed = 0;     // Time elapsed since the last FPS message [milliseconds].
    float paintElapsed = 0.f;   // Time elapsed since the last painting action [seconds].
    while (screen.isOpen()) {
        sf::Time dt {clock.restart()};
        sf::Event event;

        // Handle all events for this frame.
        while (screen.pollEvent(event)) {
            if (ShouldClose(event)) {
                Close();
                break;
            }

            if (event.type == sf::Event::KeyPressed && event.key.scancode == sf::Keyboard::Scan::D) {
                DEBUG = !DEBUG;
            }

            SetMouseState(mouse, event, sf::Mouse::getPosition(screen));
        }

        if (mouse.state) {
            mouse.pos = sf::Mouse::getPosition(screen);
            if (mouse.state == MouseState::DRAWING) {
                if (paintElapsed >= mouse.brushInfo.timeout) {
                    Paint(mouse);
                    paintElapsed =  0.f;
                }
            } else if (mouse.state == MouseState::DRAGGING) {
                // The view's position DOESN'T use the transformed world coordinates, so we need to use mapPixelToCoords.
                RepositionView(mouse);
            }
            mouse.prevPos = mouse.pos;
        }
        UpdateVisibleRooms();
        Step(dt.asSeconds());
        Draw(screen);
        // DEBUG ONLY - Draw the active chunks.
        if (DEBUG) {
            DrawChunks();
        }
        screen.display();

        paintElapsed += dt.asSeconds();
        if (fpsElapsed >= 1000) {
            std::cout << "FPS: " << 1.f / dt.asSeconds() << "\n";
            fpsElapsed = 0;
        } else {
            fpsElapsed += dt.asMilliseconds();
        }
    }
}

void SandGame::Step(float dt) {
    for (roomID_t id = 0; id < world.rooms.Range(); ++id) {
        SandWorker worker {id, world, &world.GetRoom(id)};
        worker.Step(dt);
    }
}

///////////////////////////// Game interaction functions /////////////////////////////

void SandGame::SetMouseState(Mouse &mouse, sf::Event &event, sf::Vector2i position) {
    switch (event.type) {
        case sf::Event::MouseButtonPressed:
            if      (event.mouseButton.button == sf::Mouse::Left  ) { mouse.state = MouseState::DRAWING; }
            else if (event.mouseButton.button == sf::Mouse::Middle) { mouse.state = MouseState::DRAGGING; }
            mouse.prevPos = position;
            break;
        case sf::Event::MouseButtonReleased:
            mouse.Reset();
            break;
        case sf::Event::MouseLeft:
            mouse.Reset();
            break;
        case sf::Event::MouseWheelMoved:
            if      (event.mouseWheel.delta > 0) { mouse.radius = std::clamp(mouse.radius + 1, 1, 10); }
            else if (event.mouseWheel.delta < 0) { mouse.radius = std::clamp(mouse.radius - 1, 1, 10); }
            break;
        case sf::Event::KeyPressed: {
            int number {KEY_TO_NUMBER(event.key.code)};
            if (number >= 0 && number <= Element::count - 1)
                mouse.brush = static_cast<Element>(number);
                mouse.brushInfo = world.properties.brushes[mouse.brush];
            break; }
        default:
            // Nothing
            break;
    }
}

void SandGame::Paint(Mouse &mouse) {
    sf::Vector2i end    {sf::Vector2i{screen.ToWorld(mouse.pos    )}};
    sf::Vector2i start  {sf::Vector2i{screen.ToWorld(mouse.prevPos)}};

    Lerp lerp {start, end};
    Paint(lerp, mouse.brush, std::min(mouse.radius, mouse.brushInfo.maxRadius));
}

void SandGame::Paint(Lerp &stroke, Element type, int radius) {
    for (sf::Vector2i pos : stroke) {
        if (radius == 1) {
            world.SetCell(pos.x, pos.y, type); // TODO: Cache the room that the mouse is in.
        } else {
            world.SetArea(pos.x - (radius - 1), pos.y - (radius - 1),
                            2 * radius, 2 * radius, type);
        }
    }
}

void SandGame::RepositionView(Mouse mouse) {
    sf::Vector2f delta    {screen.mapPixelToCoords(mouse.prevPos) - screen.mapPixelToCoords(mouse.pos)};

    const sf::Vector2i newPos {screen.tfInv * (delta + screen.ViewCentre())};
    sf::Vector2i size {screen.ViewBorders().getSize() / 2.f};
    size.y = size.y - 1;
    if (newPos.x - size.x < constants::roomWidth * xMinRooms || newPos.x + size.x > constants::roomWidth * xMaxRooms) {
        delta.x = 0.f;
    }
    if (newPos.y - size.y < constants::roomHeight * yMinRooms || newPos.y + size.y > constants::roomHeight * yMaxRooms - 1) {
        delta.y = 0.f;
    }

    screen.RepositionView(delta);
}

///////////////////////////// Draw functions /////////////////////////////

void SandGame::Draw(Screen &screen) {
    screen.clear();
    const sf::IntRect borders {screen.ViewBorders()};
    gridSprite.setPosition(visibleRooms[0].first.x, visibleRooms[0].first.y);

    int xMin, xMax, // Determines the potion of a room that's drawn.
        yMin, yMax;
    
    std::vector<roomID_t> completed;
    completed.reserve(4);
    for (int i = 0; i < visibleRooms.size(); ++i) {
        if (std::find(completed.begin(), completed.end(), visibleRooms[i].second) != completed.end())
            continue; // Room already processes.
        if (!VALID_ROOM(visibleRooms[i].second))
            continue;

        SandRoom &room {world.GetRoom(visibleRooms[i].second)};
        sf::Vector2i intersect {visibleRooms[i].first}; // The point in the room at which the view corner is located.
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
        int blX = visibleRooms[0].first.x, blY = visibleRooms[0].first.y; // For translating world coords to view coords.
        for (int y = yMin; y < yMax; ++y) {
        for (int x = xMin; x < xMax; ++x) {
            gridImage.setPixel(x - blX, y - blY, room.grid.colour[room.ToIndex(x, y)]);
        }
        }
        // Draw the particles.
        for (int ip = 0; ip < room.particles.Range(); ip++) {
            Particle particle {room.particles[ip]};
            sf::Vector2i position {particle.Position()};
            gridImage.setPixel(position.x - blX, position.y - blY, particle.colour);
        }
        completed.push_back(visibleRooms[i].second);
    }
    gridTexture.loadFromImage(gridImage);
    gridSprite.setTexture(gridTexture);
    screen.Draw(gridSprite);
}

void SandGame::UpdateVisibleRooms() {
    const sf::IntRect borders {screen.ViewBorders()};
    sf::Vector2i size {borders.getSize() - sf::Vector2i(1, 1)};
    sf::Vector2i corners[4] {
        borders.getPosition() - sf::Vector2( size.x,  size.y) / 2,
        borders.getPosition() - sf::Vector2(-size.x,  size.y) / 2,
        borders.getPosition() - sf::Vector2( size.x, -size.y) / 2,
        borders.getPosition() - sf::Vector2(-size.x, -size.y) / 2
    };
    std::vector<std::pair<sf::Vector2i, roomID_t>> rooms;
    rooms.reserve(4);
    for (sf::Vector2i corner : corners) {
        roomID_t roomID {world.ContainingRoomID(corner)};
        if (!VALID_ROOM(roomID)) {
            roomID = world.SpawnRoom(corner.x, corner.y);
        }
        rooms.push_back(std::make_pair(corner, roomID));
    }

    std::swap(visibleRooms, rooms);
}

void SandGame::DrawChunks() {
    sf::RectangleShape rectangle;
    rectangle.setOutlineThickness(1);
    rectangle.setFillColor(sf::Color::Transparent);
    for (roomID_t id = 0; id < world.rooms.Range(); ++id) {
        SandRoom &room {world.GetRoom(id)};
        rectangle.setSize(sf::Vector2f(room.width, room.height));
        rectangle.setOutlineColor(sf::Color::Red);
        rectangle.setPosition(room.x, room.y);
        screen.Draw(rectangle);
        for (int i = 0; i < room.chunks.Size(); i++) {
            if (room.chunks.IsActive(i)) {
                Chunk &chunk {room.chunks.GetChunk(i)};
                ChunkBounds bounds {room.chunks.GetBounds(i)};
                rectangle.setSize(sf::Vector2f(bounds.width, bounds.height));
                rectangle.setOutlineColor(sf::Color::Blue);
                rectangle.setPosition(bounds.x, bounds.y);
                screen.Draw(rectangle);
                rectangle.setSize(sf::Vector2f(chunk.xMax - chunk.xMin, chunk.yMax - chunk.yMin));
                rectangle.setOutlineColor(sf::Color::Green);
                rectangle.setPosition(chunk.xMin, chunk.yMin);
                screen.Draw(rectangle);
            }
        }
    }
}
