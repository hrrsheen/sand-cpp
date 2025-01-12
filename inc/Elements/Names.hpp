#ifndef ELEMENTS_NAMES_HPP
#define ELEMENTS_NAMES_HPP

// The element is used for accessing a cell's corresponding properties.
// Used for array indexing! Don't change numbers. count must always be the last entry.
enum Element {
    null = -1,
    air = 0,
    sand,
    stone,
    water,
    fire,
    wood,
    smoke,

    // MUST REMAIN AS LAST ELEMENT.
    count
};

#endif