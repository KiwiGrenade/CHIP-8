#include <iostream>
#include <memory>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Keyboard.hpp>
#include "chip8.hpp"
#include "utils.hpp"

bool Options::verbose = false;
bool Options::debug = false;

int main(int argc, char *argv[])
{
    if(argc < 2) {
        std::cerr << "ERROR: Too few arguments!" << std::endl;
        return 2;
    }

    std::string inputFileName;
    for(size_t i = 1; i < argc; ++i) {
        if(argv[i][0] == '-') {
            for(size_t j = 1; argv[i][j] != '\0'; j++) {
                char& c = argv[i][j];
                if(c == 'v' || c == 'V')
                    Options::verbose = true;
                else if(c == 'd' || c == 'D')
                    Options::debug = true;
            }
        }
        else {
            inputFileName = argv[i];
        }
    }

    if(inputFileName.empty()) {
        std::cerr << "ERROR: No input file path!" << std::endl;
        return 2;
    }


    sf::RenderWindow window = sf::RenderWindow{ { 640u, 320u}, "CHIP-8"};
    window.setFramerateLimit(60);
    
    std::unique_ptr<Chip8> myChip8;
    myChip8 = std::make_unique<Chip8>();
    myChip8->loadFile(inputFileName);
    if(Options::verbose)
        myChip8->memory->printProgram();

    while (window.isOpen())
    {
        sf::Event event = sf::Event{};
        while(window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (   event.type == sf::Event::KeyReleased
                    &&  event.key.scancode == sf::Keyboard::Scan::Enter) {
                myChip8->emulateCycle();
                if(myChip8->drawFlag)
                    myChip8->drawScreen(window);
            }
        }
        myChip8->emulateCycle();
        if(myChip8->drawFlag)
            myChip8->drawScreen(window);
    }

    return 0;
}
