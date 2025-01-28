#ifndef CELL_HPP
#define CELL_HPP

#include "Constants.hpp"
#include "Elements/Names.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>

struct ElementProperties;
struct ConstProperties;
class ActionWorker;

struct CellState {
    Element id;
    float health            = 100.f;
    sf::Vector2f velocity   = sf::Vector2f(0.f, 0.f);

    CellState() : id(Element::air) {}
    CellState(Element _id) : id(_id) {}
    void ApplyAcceleration(sf::Vector2f acc, float dt);
};

class Cells {
public:
    std::vector<CellState> state;
    std::vector<sf::Color> colour;

private:
    ElementProperties const *properties;

public:
    Cells(int width, int height, const ElementProperties *_properties);

    //////// Assignment / manipulation functions ////////
    void Assign(size_t i, Element id, sf::Color newColour);
    void Assign(size_t i, Element id, int x=0, int y=0);

    // Properties queries.
    const ConstProperties& GetProperties(int index) const;
    bool CanDisplace(Element self, Element other) const;
    int SpreadRate(size_t i) const;
    float Flammability(size_t i) const;
};

#endif