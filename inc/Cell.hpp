#ifndef CELL_HPP
#define CELL_HPP

#include "Actions.hpp"
#include "Elements/Names.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>

struct ElementProperties;

struct CellState {
    Element id;
    float health;
    sf::Vector2f velocity;

    CellState() : id(Element::air), health(100.f), velocity(0.f, 0.f) {}
    CellState(Element _id) : id(_id), health(100.f), velocity(0.f, 0.f) {}
    void ApplyAcceleration(sf::Vector2f acc, float dt);
};

class Cells {
public:
    std::vector<CellState> state;
    std::vector<sf::Color> colour;

private:
    std::vector<std::pair<size_t, Element>> queuedTransforms;

    ElementProperties const *properties;

public:
    Cells(int width, int height, const ElementProperties *_properties);

    //////// Assignment / manipulation functions ////////
    void Assign(size_t i, Element id, int x=0, int y=0);

    // Properties queries.
    bool CanDisplace(Element self, Element other) const;
    int SpreadRate(size_t i) const;
    int Flammability(size_t i) const;

private:
    Action ActOnSelf    (sf::Vector2i p, float dt);
    Action ActOnOther   (sf::Vector2i p, sf::Vector2i otherP, size_t other, float dt);
};

#endif