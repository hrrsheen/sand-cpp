#include "cell.hpp"
#include "grid.hpp"
#include "helpers.hpp"
#include <cmath>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

////////////////////////////////////////////////////////// Cell Properties //////////////////////////////////////////////////////////

void ElementProperties::loadFromFile(std::string_view fileName) {
    // PLACEHOLDER for before I add JSON support.
    if (fileName == "air") {
        id = 0;
        type = ElementType::air;
        name = "air";
        std::get<colourIndex>(palette).push_back(0x000000ff);
    } else if (fileName == "sand") {
        id = 1; // TODO: Add checking that ID is unique.
        type = ElementType::solidMovable;
        name = "sand";
        std::get<colourIndex>(palette).push_back(0xfabf73ff);
        std::get<colourIndex>(palette).push_back(0xebae60ff);
    } else if (fileName == "stone") {
        id = 2;
        type = ElementType::solidImmovable;
        name = "stone";
        sf::Image img;
        img.loadFromFile("./assets/stone-texture.png");
        palette = img;
    }
}

sf::Color ElementProperties::colourFromArray() {
    if (std::get<colourIndex>(palette).size() == 0) {
        return sf::Color(0x00000000);
    }
    if (std::get<colourIndex>(palette).size() == 1) {
        return sf::Color(std::get<colourIndex>(palette).at(0)); // Trivial case where elements are a single colour.
    }
    int position {quickRandInt(std::get<colourIndex>(palette).size())};
    return sf::Color(std::get<colourIndex>(palette).at(position));
}

sf::Color ElementProperties::colourFromTexture(sf::Vector2i p) {
    sf::Vector2u size {std::get<textureIndex>(palette).getSize()};
    int x {p.x % static_cast<int>(size.x)};
    int y {p.y % static_cast<int>(size.y)};
    return std::get<textureIndex>(palette).getPixel(std::abs(x), std::abs(y));
}

bool ElementProperties::hasTexture() {
    return palette.index() == textureIndex;
}

bool ElementProperties::canDisplace(ElementProperties &other) {
    if (type == ElementType::solidMovable && (other.type == ElementType::solidMovable || other.type == ElementType::solidImmovable)) {
        return false;
    }

    if (other.type == ElementType::air) {
        return true;
    }

    return density > other.density;
}

bool ElementProperties::operator==(const ElementProperties &otherProperty) const {
    return id == otherProperty.id || name == otherProperty.name;
}


size_t PropertiesContainer::insert(const ElementProperties &properties) {
    if (contains(properties)) {
        return -1;
    }

    elementProperties.push_back(properties);
    return elementProperties.size() - 1;
}

ElementProperties& PropertiesContainer::get(size_t index) {
    return elementProperties.at(index);
}

bool PropertiesContainer::contains(const ElementProperties &properties) {
    for (ElementProperties &ep : elementProperties) {
        if (ep == properties) return true;
    }

    return false;
}


////////////////////////////////////////////////////////// Cells //////////////////////////////////////////////////////////


Cell::Cell() : elementId(-1), p(0, 0), redraw(false) {
}

void Cell::assign(int elementId, ElementProperties &properties) {
    Cell::elementId = elementId;
    if (properties.hasTexture()) {
        colour = properties.colourFromTexture(p);
    } else {
        colour = properties.colourFromArray();
    }
    redraw = true;
}

void swap(Cell *cellA, Cell *cellB) {
    Cell tmpCell = *cellA;
    cellA->elementId = cellB->elementId;
    cellA->colour = cellB->colour;
    
    cellB->elementId = tmpCell.elementId;
    cellB->colour = tmpCell.colour;
}