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
enum class ElementType : uint8_t {
    null,
    air,
    solidMovable,
    solidImmovable,
    liquid,
    gas
};

struct ElementProperties {
public:
    //////// Initialisation functions ////////
    ElementProperties();
    ElementProperties(ElementType thisType);
    ElementProperties(Element thisId, ElementType thisType, std::string_view thisName);

    // Returns a colour, picked at random from the palette.
    sf::Color colourFromArray();

    // Returns a colour from a texture that corresponds to the supplied position.
    sf::Color colourFromTexture(sf::Vector2i p);

    // Returns true if a texture is available for cells with these properties.
    bool hasTexture();

    //////// Simulation functions ////////

    // Returns true if the element represented by these properties can displace the element
    // represented by the other properties. False otherwise.
    virtual bool canDisplace(ElementProperties &other);

    //////// Operators ////////
    bool operator==(const ElementProperties &otherProperty) const;

    const Element id;         // Identifier of the elment that these properties represent. MUST be unique.
    const ElementType type;   // Detemines the movement behaviour of the element that these properties represent.
    const std::string name;   // The name of the element that these properties represent. MUST be unique.

    // Display.
    std::variant<std::vector<sf::Uint32>, sf::Image> palette;

    // Simulation.
    virtual const float density() const { return 0.f; }
    virtual const bool isMobile() const { return false; }

    // Access constants.
    static const int colourIndex {0};
    static const int textureIndex {1};

    friend class Cell;
};



///////////////////////////////////////////// Container /////////////////////////////////////////////
class PropertiesContainer {
public:
    PropertiesContainer();

    // Populates the properties' values from a file.
    void loadFromFile(std::string_view fileName);

    // Inserts a property into the container. If a properties class with the same 
    // id already exists in the container, no insertion will occur.
    // Returns true if inertion was successful, false otherwise.
    bool insert(std::unique_ptr<ElementProperties> properties);

    // Retrieves the ElementProperties from the given index.
    ElementProperties& get(Element key);

private:
    // Returns true if another properties with the same ID already exists in the container.
    bool contains(const ElementProperties &properties);

    std::vector<std::unique_ptr<ElementProperties>> elementProperties;
};

#endif