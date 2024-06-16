#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <memory>
#include <string>
#include "chip8.hpp"

int main()
{
    std::string welcomeMessage {"CHIP-8 will be here soon! WORK IN PROGRESS"};

    sf::RenderWindow window = sf::RenderWindow{ { 640u, 320u}, "CHIP-8"};
    window.setFramerateLimit(60);
    
    std::unique_ptr<Chip8> myChip8;
    myChip8 = std::make_unique<Chip8>(); 
    myChip8->initialize();
    myChip8->loadFile("../roms/test/1-chip8-logo.ch8");

    while (window.isOpen())
    {
        for (auto event = sf::Event{}; window.pollEvent(event);)
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        myChip8->emulateCycle();

        if(myChip8->drawFlag)
            myChip8->drawScreen(window);
    }

    return 0;
}
