#include "elements/element_properties.hpp"
#include "elements/solid_immovable.hpp"
#include <SFML/Graphics.hpp>

SolidImmovable::SolidImmovable() : ElementProperties(ElementType::solidImmovable) {}
SolidImmovable::SolidImmovable(Element thisId, std::string_view thisName) : 
    ElementProperties(thisId, ElementType::solidImmovable, thisName) {}

Stone::Stone() : SolidImmovable(Element::stone, "stone") {
    sf::Image img;
    img.loadFromFile("./assets/stone-texture.png");
    palette = img;
}