#include "Helpers.hpp"
#include "SandGame.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    InitRng();
    SandGame game;
    
    game.Run();
}