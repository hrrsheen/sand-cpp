#ifndef ELEMENT_PROPERTIES_HPP
#define ELEMENT_PROPERTIES_HPP

#include <cstdint>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <string>
#include <variant>
#include <vector>
#include <unordered_map>

// The element is used for accessing a cell's corresponding properties.
// Used for array indexing! Don't change numbers. count must always be the last entry.
enum class Element {
    null = -1,
    air = 0,
    sand,
    stone,
    water,
    fire,

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
    ACCELERATE_DOWN
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
    const ElementType type;   // Detemines the movement behaviour of the element that these properties represent.
    const std::string name;   // The name of the element that these properties represent. MUST be unique.
    const MoveType moveBehaviour;
    const uint8_t spreadBehaviour;

    // Display.
    std::variant<std::vector<sf::Uint32>, sf::Image> palette;

    // Access constants.
    static const int colourIndex {0};
    static const int textureIndex {1};

    //////// Initialisation functions ////////
    ElementProperties();
    ElementProperties(ElementType thisType);
    ElementProperties(Element thisId, ElementType thisType, std::string_view thisName);
    ElementProperties(Element thisId, ElementType thisType, std::string_view thisName, MoveType movement, uint8_t spread);

    // Returns a colour, picked at random from the palette.
    sf::Color ColourFromArray();

    // Returns a colour from a texture that corresponds to the supplied position.
    sf::Color ColourFromTexture(sf::Vector2i p);

    // Returns true if a texture is available for cells with these properties.
    bool HasTexture();

    //////// Simulation functions ////////

    // Returns true if the element represented by these properties can displace the element
    // represented by the other properties. False otherwise.
    virtual bool canDisplace(ElementProperties &other) const;

    //////// Operators ////////
    bool operator==(const ElementProperties &otherProperty) const;

    friend class Cell;
};

#endif