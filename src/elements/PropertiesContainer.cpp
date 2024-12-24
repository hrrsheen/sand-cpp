#include "Elements/ElementProperties.hpp"
#include "Elements/PropertiesContainer.hpp"

PropertiesContainer::PropertiesContainer() {
    elementProperties.resize(static_cast<int>(Element::count) + 1);

    // Add the default element properties.
    // Might need to make a class for air?
    std::unique_ptr<ElementProperties> air {std::make_unique<ElementProperties>(Element::air, ElementType::AIR, "air")};
    std::get<ElementProperties::colourIndex>(air->palette).push_back(0x000000ff);
    Insert(std::move(air));
}

bool PropertiesContainer::Insert(std::unique_ptr<ElementProperties> properties) {
    if (Contains(*properties)) {
        return false;
    }

    elementProperties[static_cast<int>(properties->id)] = std::move(properties);
    return true;
}

ElementProperties& PropertiesContainer::Get(Element key) {
    return *elementProperties.at(static_cast<int>(key));
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