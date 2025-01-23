#ifndef ELEMENTS_INITS_HPP
#define ELEMENTS_INITS_HPP

#include "Elements/ElementProperties.hpp"

//////// Solid inits ////////
bool InitSand   (ElementProperties &properties);
bool InitStone  (ElementProperties &properties);
bool InitWood   (ElementProperties &properties);

//////// Liquid inits ////////
bool InitWater  (ElementProperties &properties);

//////// Gas inits ////////
bool InitFire   (ElementProperties &properties);
bool InitSmoke  (ElementProperties &properties);
bool InitExplosion(ElementProperties &properties);
bool InitSpark  (ElementProperties &properties);

#endif