#include <iostream>
#include <memory>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Keyboard.hpp>
#include "chip8.hpp"

int main(int argc, char *argv[])
{
    Mode mode = Mode::Normal;

    if(argc < 2) {
        std::cout << "ERROR: Too few arguments!" << std::endl;
        return 2;
    }
    
    // TODO: Add normal argument parsing
    if(argc > 2)
        mode = Mode::Verbose;

    std::string inputFileName = argv[1];

    sf::RenderWindow window = sf::RenderWindow{ { 640u, 320u}, "CHIP-8"};
    window.setFramerateLimit(60);
    
    std::unique_ptr<Chip8> myChip8;
    myChip8 = std::make_unique<Chip8>(mode); 
    myChip8->loadFile(inputFileName);
    myChip8->memory->printProgram();

    while (window.isOpen())
    {
        sf::Event event = sf::Event{};
        while(window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            // else if (event.type == sf::Event::KeyReleased) {
                // if (event.key.scancode == sf::Keyboard::Scan::Enter) {
                    // myChip8->emulateCycle();
                    // if(myChip8->drawFlag)
                    //     myChip8->drawScreen(window);
            //     }
            // }
        }
        myChip8->emulateCycle();
        if(myChip8->drawFlag)
            myChip8->drawScreen(window);

    }

    return 0;
}
