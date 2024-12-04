#include "elements/element_properties.hpp"
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
ElementProperties::ElementProperties() : 
    id(Element::null), 
    type(ElementType::null),
    name() {
}

ElementProperties::ElementProperties(ElementType thisType) : id(), type(thisType), name() {}

ElementProperties::ElementProperties(Element thisId, ElementType thisType, std::string_view thisName) :
    id(thisId), type(thisType), name(thisName) {}

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
    return false;
}

bool ElementProperties::operator==(const ElementProperties &otherProperty) const {
    if (!this)
    return id == otherProperty.id || name == otherProperty.name;
}


///////////////////////////////////////////// Container /////////////////////////////////////////////
PropertiesContainer::PropertiesContainer() {
    elementProperties.resize(static_cast<int>(Element::count) + 1);

    // Add the default element properties.
    // Might need to make a class for air?
    std::unique_ptr<ElementProperties> air {std::make_unique<ElementProperties>(Element::air, ElementType::air, "air")};
    std::get<ElementProperties::colourIndex>(air->palette).push_back(0x000000ff);
    insert(std::move(air));
}

bool PropertiesContainer::insert(std::unique_ptr<ElementProperties> properties) {
    if (contains(*properties)) {
        return false;
    }

    // elementProperties.insert(elementProperties.begin() + static_cast<int>(properties.id), properties);
    elementProperties[static_cast<int>(properties->id)] = std::move(properties);
    return true;
}

ElementProperties& PropertiesContainer::get(Element key) {
    return *elementProperties.at(static_cast<int>(key));
}

bool PropertiesContainer::contains(const ElementProperties &properties) {
    for (auto &ep : elementProperties) {
        if (!ep) continue;
        if (ep->id == properties.id || ep->name == properties.name) {
            return true;
        }
    }

    return false;
}
