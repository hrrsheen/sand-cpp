#ifndef ELEMENT_PROPERTIES_HPP
#define ELEMENT_PROPERTIES_HPP

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
/**
 * Contains information about the properties of an element that are used for simulation.
 */
    // Movement / Interactions.
    ElementType type;
    std::string name;               // The name of the element that these properties represent. MUST be unique.
    MoveType    moveBehaviour;      // Defines the behaviour of the element in freefall.
    uint8_t     spreadBehaviour;    // Defines the behaviour of the element when space below it is occupied.
    moveset_t   actionSet;          // The set of the relative positions of the cells that this element can act upon (burn, corrode, etc).

    // Simulation properties.
    int     spreadRate      = 1;    // The rate at which liquids spread.
    float   flammability    = 0.f;  // Determines how easily an element can catch fire.
    float   hardness        = 0.f;  // Used for resisting explosions.

    ConstProperties();
};

struct ColourProperties {
/**
 * Contains information about how a given element is displayed when drawing.
 */
    bool colourEachFrame;
    std::variant<std::vector<sf::Uint32>, sf::Image> palette;

    ColourProperties() : colourEachFrame(false), palette() {}
};

struct PaintProperties {
/**
 * Contains information about how a given element is painted into the world with a mouse.
 */
    // Time allowed between paintings of a given element.
    float timeout = 0.f;
    // The maximum brush size for a given element.
    int maxRadius = std::numeric_limits<int>::max();
};

struct ElementProperties {
public:
    std::vector<ConstProperties>    constants;
    std::vector<ColourProperties>   colours;
    std::vector<PaintProperties>    brushes;

public:
    //////// Initialisation functions ////////
    ElementProperties();
    // Returns true if insertion was successful, false otherwise.
    bool Insert(Element id, ConstProperties consts, ColourProperties palette, PaintProperties brush={});

    //////// Display functions ////////
    sf::Color Colour(Element id, int x=0, int y=0) const;

    //////// Simulation functions ////////
    // Returns true if the element represented by these properties can displace the element
    // represented by the other properties. False otherwise.
    bool CanDisplace(Element self, Element other) const;
    bool CanDisplace(ElementType self, ElementType other) const;

private:
    sf::Color ColourFromArray(Element id) const;                    // Returns a colour, picked at random from the palette.
    sf::Color ColourFromTexture(Element id, int x, int y) const;    // Returns a colour from a texture that corresponds to the supplied position.
    bool HasTexture(Element id) const;                              // Returns true if a texture is available for cells with these properties.

    bool Contains(Element id) const;
};

#endif