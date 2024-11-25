#include "cell.hpp"
#include "grid.hpp"
#include "helpers.hpp"
#include <cmath>
#include <fstream>
#include <nlohmann/json.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

using json = nlohmann::json;

////////////////////////////////////////////////////////// Cell Properties //////////////////////////////////////////////////////////

NLOHMANN_JSON_SERIALIZE_ENUM(ElementType, {
    {null, "null"},
    {air, "air"},
    {solidMovable, "solidMovable"},
    {solidImmovable, "solidImmovable"},
    {liquid, "liquid"},
    {gas, "gas"},
})

void from_json(const json &j, ElementProperties &ep) {
    j.at("id").get_to(ep.id);
    j.at("name").get_to(ep.name);
    j.at("type").get_to<ElementType>(ep.type);
    if (j.contains("colours")) {
        std::vector<std::string> hexStrings{};
        j.at("colours").get_to<std::vector<std::string>>(hexStrings);
        for (std::string &str : hexStrings) {
            sf::Uint32 hex {static_cast<sf::Uint32>(std::stoul(str, nullptr, 16))};
            std::get<ElementProperties::colourIndex>(ep.palette).push_back(hex);
        }
    } else if (j.contains("texture")) {
        sf::Image img;
        img.loadFromFile(j.at("texture").get<std::string>());
        ep.palette = img;
    } else {
        // :(
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


PropertiesContainer::PropertiesContainer() {
    // Add the default element properties.
    ElementProperties epAir {};
    epAir.id = 0;
    epAir.name = "air";
    epAir.type = ElementType::air;
    std::get<ElementProperties::colourIndex>(epAir.palette).push_back(0x000000ff);
    insert(epAir);
}

void PropertiesContainer::loadFromFile(std::string_view fileName) {
    std::ifstream f(fileName);
    json data = json::parse(f);

    std::vector<ElementProperties> eps {};
    data.at("elements").get_to<std::vector<ElementProperties>>(eps);
    for (ElementProperties &ep : eps) {
        insert(ep);
    }
}

size_t PropertiesContainer::insert(const ElementProperties &properties) {
    if (contains(properties) || properties.id == -1) {
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