#include "Elements/ElementProperties.hpp"
#include "Elements/PropertiesContainer.hpp"

PropertiesContainer::PropertiesContainer() {
    elementProperties.resize(Element::count + 1);

    // Add the default element properties.
    // Might need to make a class for air?
    ConstProperties init;
    init.type = ElementType::AIR;
    init.name = "air";
    COLOUR(init.palette).push_back(0x000000ff);
    std::unique_ptr<ElementProperties> air {std::make_unique<ElementProperties>(Element::air, init)};
    
    Insert(std::move(air));
}

bool PropertiesContainer::Insert(std::unique_ptr<ElementProperties> properties) {
    if (Contains(*properties)) {
        return false;
    }

    elementProperties[properties->id] = std::move(properties);
    return true;
}

ElementProperties& PropertiesContainer::Get(Element id) {
    return *elementProperties.at(id);
}

const ElementProperties& PropertiesContainer::Get(Element id) const {
    return *elementProperties.at(id);
}

size_t PropertiesContainer::Size() const {
    return elementProperties.size();
}

bool PropertiesContainer::Contains(const ElementProperties &properties) {
    for (auto &ep : elementProperties) {
        if (!ep) continue;
        if (ep->id == properties.id || ep->name == properties.name) {
            return true;
        }
    }

    return false;
}