#ifndef CELL_HPP
#define CELL_HPP

#include <cstdint>
#include <SFML/Config.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>
#include <variant>
#include <vector>
#include <unordered_set>

class Grid;

////////////////////////////////////////////////////////// Cell Properties //////////////////////////////////////////////////////////

enum ElementType : uint8_t{
    null,
    air,
    solidMovable,
    solidImmovable,
    liquid,
    gas
};

struct ElementProperties {
    //////// Initialisation functions ////////

    // Populates the properties' values from a file.
    void loadFromFile(std::string_view fileName);

    // Returns a colour, picked at random from the palette.
    sf::Color colourFromArray();

    // Returns a colour from a texture that corresponds to the supplied position.
    sf::Color colourFromTexture(sf::Vector2i p);

    // Returns true if a texture is available for cells with these properties.
    bool hasTexture();

    //////// Simulation functions ////////

    // Returns true if the element represented by these properties can displace the element
    // represented by the other properties. False otherwise.
    bool canDisplace(ElementProperties &other);

    //////// Operators ////////
    bool operator==(const ElementProperties &otherProperty) const;

    int id;             // Identifier of the elment that these properties represent. MUST be unique.
    ElementType type;   // Detemines the movement behaviour of the element that these properties represent.
    std::string name;   // The name of the element that these properties represent. MUST be unique.

    // Display.
    // union Palette
    // {
    //     std::vector<sf::Uint32> colours;
    //     sf::Image texture;
    // };
    std::variant<std::vector<sf::Uint32>, sf::Image> palette;
    
    // Palette palette;

    // Simulation.
    float spreadRate;
    float density;

private:
    static const int colourIndex {0};
    static const int textureIndex {1};
};

class PropertiesContainer {
public:

    // Inserts a property into the container. If a properties class with the same 
    // id already exists in the container, no insertion will occur.
    // If successful, returns its index within the container. Returns -1 otherwise.
    size_t insert(const ElementProperties &properties);

    // Retrieves the ElementProperties from the given index.
    ElementProperties& get(size_t index);

private:
    // Returns true if another properties with the same ID already exists in the container.
    bool contains(const ElementProperties &properties);

    std::vector<ElementProperties> elementProperties;
};

////////////////////////////////////////////////////////// Cells //////////////////////////////////////////////////////////

class Cell {
public:
    Cell();

    void assign(int elementId, ElementProperties &properties);

    friend void swap(Cell &, Cell &);

    int elementId;      // The ID (position in the container for now) of the element that's contained within this cell.
    sf::Vector2i p;     // The (x, y) coordinate of this cell within its grid.
    sf::Color colour;   // The colour of this cell.

    bool redraw;        // Tracks whether this cell has changes since the previous frame.
};

// Swaps the cell types of cellA and cellB.
void swap(Cell *cellA, Cell *cellB);

#endif