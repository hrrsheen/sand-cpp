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
                       yMinRooms( 0), yMaxRooms(3),
                       world(-2, 2, 0, 3, 6),
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

    font.loadFromFile("./assets/pixel-font.otf");
    text.setFont(font);
    text.setFillColor(sf::Color::White);
    text.setScale(sf::Vector2f {0.45f, 0.45f});
}

void SandGame::Run() {
    bool DEBUG = false;
    char fpsBuffer[10];

    sf::Clock clock;
    Mouse mouse;
    float fpsTarget = 10000.f;  // The maximum FPS that the screen will be drawn to.
    int     fpsElapsed = 0;     // Time elapsed since the last FPS message [milliseconds].
    float paintElapsed = 0.f;   // Time elapsed since the last painting action [seconds].
    float frameElapsed = 0.f;   // Time elapsed since the last frame was drawn (not simulated) [seconds].
    while (screen.isOpen()) {
        // Update timers.
        sf::Time dt {clock.restart()};
        frameElapsed += dt.asSeconds();
        paintElapsed += dt.asSeconds();

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
                text.setPosition(screen.ViewCentre() - sf::Vector2f {256.f, 128.f});
            }
            mouse.prevPos = mouse.pos;
        }
        UpdateVisibleRooms();
        Step(dt.asSeconds());

        if (frameElapsed > 1.f / fpsTarget) {
            Draw(screen);
            // DEBUG ONLY - Draw the active chunks.
            if (DEBUG) { DrawChunks(); }
            screen.draw(text);
            screen.display();
            frameElapsed = 0.f;
        }

        if (fpsElapsed >= 100) {
            sprintf(fpsBuffer, "%6d", static_cast<int>(1.f / dt.asSeconds()));
            text.setString(std::string(fpsBuffer));
            fpsElapsed = 0;
        } else {
            fpsElapsed += dt.asMilliseconds();
        }
    }
}

void SandGame::Step(float dt) {
    if (dt > 1 / 60.f) dt = 1 / 60.f; // DEBUG: Possibly remove this.
    for (roomID_t id = 0; id < world.rooms.Range(); ++id) {
        SandWorker worker {id, world, &world.GetRoom(id), dt};
        worker.Step();
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
    sf::Vector2f delta {screen.mapPixelToCoords(mouse.prevPos) - screen.mapPixelToCoords(mouse.pos)};

    const sf::Vector2i newPos {screen.tfInv * (delta + screen.ViewCentre())};
    sf::Vector2i size {screen.ViewDimensions().getSize() / 2.f};
    size.y = size.y - 1;

    // Check that the new view position would not extend outside the world limits.
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
    gridSprite.setPosition(visibleRooms[0].first.x, visibleRooms[0].first.y); // Update the sprite to sit under the view.

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
            Particle& particle {room.particles[ip]};
            sf::Vector2i position {particle.Position()};
            // Only draw particles that are inside the view.
            if (position.x >= xMin && position.x < xMax && position.y >= yMin && position.y < yMax)
                gridImage.setPixel(position.x - blX, position.y - blY, particle.colour);
        }
        completed.push_back(visibleRooms[i].second);
    }
    gridTexture.loadFromImage(gridImage);
    gridSprite.setTexture(gridTexture);
    screen.Draw(gridSprite);
}

void SandGame::UpdateVisibleRooms() {
    const sf::IntRect dimensions {screen.ViewDimensions()};
    sf::Vector2i size {dimensions.getSize() - sf::Vector2i(1, 1)};

    // Get the position of each view corner in world space.
    sf::Vector2i corners[4] {
        dimensions.getPosition() - sf::Vector2( size.x,  size.y) / 2,   // Bottom Left
        dimensions.getPosition() - sf::Vector2(-size.x,  size.y) / 2,   // Bottom Right
        dimensions.getPosition() - sf::Vector2( size.x, -size.y) / 2,   // Top Left
        dimensions.getPosition() - sf::Vector2(-size.x, -size.y) / 2    // Top Right
    };

    std::vector<std::pair<sf::Vector2i, roomID_t>> rooms;
    rooms.reserve(4);
    // Make a vector that pairs each corner position with the ID of the room that contains it.
    for (sf::Vector2i corner : corners) {
        roomID_t roomID {world.ContainingRoomID(corner)};
        if (!VALID_ROOM(roomID)) {
            roomID = world.SpawnRoom(corner.x, corner.y);
        }
        rooms.push_back(std::make_pair(corner, roomID));
    }

    // Update the member vector.
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
