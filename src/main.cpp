#include "SandGame.hpp"
#include "Utility/Random.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    InitRng();
    SandGame game;
    
    game.Run();

    return 0;
}