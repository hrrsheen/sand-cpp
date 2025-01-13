#ifndef ELEMENT_PROPERTIES_HPP
#define ELEMENT_PROPERTIES_HPP

#include "Actions.hpp"
#include "Elements/Names.hpp"
#include <cstdint>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <string>
#include <variant>
#include <vector>
#include <unordered_map>

#define COLOUR(x)   std::get<0>(x)
#define TEXTURE(x)  std::get<1>(x)

typedef std::vector<sf::Vector2i> moveset_t;

// Forward declarations.
class Cell;
class PropertiesContainer;
class SandRoom;
class SandWorld;
// Forward declarations of elements.
class Solid;
class Liquid;
class Gas;

// The element type is used for determining certain interaction behaviour (such as displacement) of a cell.
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

struct ConstProperties {
    // Movement / Interactions.
    ElementType type;
    std::string name;               // The name of the element that these properties represent. MUST be unique.
    MoveType    moveBehaviour;      // Defines the behaviour of the element in freefall.
    uint8_t     spreadBehaviour;    // Defines the behaviour of the element when space below it is occupied.
    moveset_t   actionSet;          // The set of the relative positions of the cells that this element can act upon (burn, corrode, etc).

    // Display.
    bool colourEachFrame;
    std::variant<std::vector<sf::Uint32>, sf::Image> palette;

    // Simulation properties.
    int     spreadRate;
    float   flammability;

    ConstProperties();
};

struct ElementProperties {
public:
    const Element id;               // Identifier of the elment that these properties represent. MUST be unique.
    const ElementType type;         // Detemines the displacement behaviour of the element that these properties represent.
    const std::string name;         // The name of the element that these properties represent. MUST be unique.
    const MoveType moveBehaviour;   // Defines the behaviour of the element in freefall.
    const uint8_t spreadBehaviour;  // Defines the behaviour of the element when space below it is occupied.
    const moveset_t actionSet;      // The set of the relative positions of the cells that this element can act upon (burn, corrode, etc).

    // Display.
    const bool colourEachFrame; // Flag for whether this element should have a new colour selected each frame.
    const std::variant<std::vector<sf::Uint32>, sf::Image> palette;

    // Simulation constants.
    const int     spreadRate;
    const float   flammability;

    typedef const moveset_t (*moveset_gen)(void);

public:
    //////// Initialisation functions ////////
    ElementProperties(Element _id, ConstProperties &inits);

    //////// Display functions ////////
    sf::Color ColourFromArray() const;                    // Returns a colour, picked at random from the palette.
    sf::Color ColourFromTexture(int x, int y) const;      // Returns a colour from a texture that corresponds to the supplied position.
    bool HasTexture() const;                              // Returns true if a texture is available for cells with these properties.

    //////// Simulation functions ////////
    virtual Action ActUponSelf  (sf::Vector2i p, Cell &self, float dt) const;
    virtual Action ActUponOther (Cell &self,  ElementProperties &selfProp,
                                 Cell &other, ElementProperties &otherProp,
                                 sf::Vector2i p, sf::Vector2i otherP, 
                                 float dt) const;

    // Returns true if the element represented by these properties can displace the element
    // represented by the other properties. False otherwise.
    virtual bool CanDisplace(ElementType other) const;

    bool operator==(const ElementProperties &otherProperty) const;
};

#endif