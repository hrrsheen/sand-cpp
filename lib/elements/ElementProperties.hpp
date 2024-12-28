#ifndef ELEMENT_PROPERTIES_HPP
#define ELEMENT_PROPERTIES_HPP

#include <cstdint>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <string>
#include <variant>
#include <vector>
#include <unordered_map>

// Forward declarations.
class Cell;
class PropertiesContainer;
class SandWorld;
// Forward declarations of elements.
class Solid;
class Liquid;
class Gas;

// The element is used for accessing a cell's corresponding properties.
// Used for array indexing! Don't change numbers. count must always be the last entry.
enum class Element {
    null = -1,
    air = 0,
    sand,
    stone,
    water,
    fire,
    smoke,

    // MUST REMAIN AS LAST ELEMENT.
    count
};

// The element type is used for determining the movement behaviour of a cell.
enum class ElementType {
    AIR,
    SOLID,
    LIQUID,
    GAS
};

enum class MoveType {
    NONE,
    FLOAT_DOWN,
    FLOAT_UP,
    FALL_DOWN
};

enum SpreadType : uint8_t {
    NONE        = 0b00000000,
    DOWN_SIDE   = 0b00000001,
    SIDE        = 0b00000010,
    UP_SIDE     = 0b00000100
};

struct ElementProperties {
public:
    const Element id;         // Identifier of the elment that these properties represent. MUST be unique.
    const ElementType type;   // Detemines the displacement behaviour of the element that these properties represent.
    const std::string name;   // The name of the element that these properties represent. MUST be unique.
    const MoveType moveBehaviour;
    const uint8_t spreadBehaviour;

    // Display.
    std::variant<std::vector<sf::Uint32>, sf::Image> palette;

protected:
    // Access constants.
    static const int COLOUR_INDEX  = 0;
    static const int TEXTURE_INDEX = 1;
    // Simulation constants.
    static constexpr float ACCELERATION = 2000.f;
public:
    //////// Initialisation functions ////////
    ElementProperties();
    ElementProperties(ElementType thisType);
    ElementProperties(Element thisId, ElementType thisType, std::string_view thisName);
    ElementProperties(Element thisId, ElementType thisType, std::string_view thisName, MoveType movement, uint8_t spread);

    //////// Display functions ////////
    sf::Color ColourFromArray();                    // Returns a colour, picked at random from the palette.
    sf::Color ColourFromTexture(int x, int y);      // Returns a colour from a texture that corresponds to the supplied position.
    bool HasTexture();                              // Returns true if a texture is available for cells with these properties.
    // Flag for whether this element should have a new colour selected each frame.
    // This MAY ONLY be set to true for elements that have a colour palette and NOT a texture.
    virtual const bool RecolourEachFrame() const { return false; }

    //////// Movement functions ////////
    bool FloatDown      (sf::Vector2i p, Cell &cell, SandWorld &world);
    bool FloatUp        (sf::Vector2i p, Cell &cell, SandWorld &world);
    bool FallDown       (sf::Vector2i p, Cell &cell, SandWorld &world, float dt);

    bool SpreadDownSide (sf::Vector2i p, Cell &cell, SandWorld &world);
    bool SpreadUpSide   (sf::Vector2i p, Cell &cell, SandWorld &world);
    bool SpreadSide     (sf::Vector2i p, Cell &cell, SandWorld &world);

    //////// Simulation functions ////////
    virtual bool ActUponSelf(sf::Vector2i p, Cell &self, SandWorld &world, float dt);
    virtual bool ActUponNeighbours(sf::Vector2i p, SandWorld &world);

    // Returns true if the element represented by these properties can displace the element
    // represented by the other properties. False otherwise.
    virtual bool CanDisplace(ElementProperties &other) const;
            bool CanDisplace(sf::Vector2i p, SandWorld &world) const;

    //////// Operators ////////
    bool operator==(const ElementProperties &otherProperty) const;

    // Virtual constants to be overwritten by specific elements.
    virtual const int SpreadRate() const { return 1; }

    friend class Cell;
    friend class PropertiesContainer;
};

#endif