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

    uint64_t data           = 0; // Miscellaneous data to be used on a per-cell basis.

    CellState() : id(Element::air) {}
    CellState(Element _id) : id(_id) {}
    void ApplyAcceleration(sf::Vector2f acc, float dt);
};

class Cells {
public:
    std::vector<CellState> state;
    std::vector<sf::Color> colour;

public:
    Cells(int width, int height);

    //////// Assignment / manipulation functions ////////
    void Assign(size_t i, Element id, sf::Color newColour);

    void Darken(size_t i);
};

#endif