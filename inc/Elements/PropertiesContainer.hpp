#ifndef PROPERTIES_CONTAINER_HPP
#define PROPERTIES_CONTAINER_HPP

#include "Elements/ElementProperties.hpp"
#include <vector>

class PropertiesContainer {
private:
    // The container of all properties.
    std::vector<std::unique_ptr<ElementProperties>> elementProperties;

public:
    PropertiesContainer();

    // Inserts a property into the container. If a properties class with the same 
    // id already exists in the container, no insertion will occur.
    // Returns true if inertion was successful, false otherwise.
    bool Insert(std::unique_ptr<ElementProperties> properties);

    // Retrieves the ElementProperties from the given index.
    ElementProperties& Get(Element id);
    const ElementProperties& Get(Element id) const;

    size_t Size() const;

private:
    // Returns true if another properties with the same ID already exists in the container.
    bool Contains(const ElementProperties &properties);
};

#endif