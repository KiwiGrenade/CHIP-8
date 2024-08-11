#include <memory>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Keyboard.hpp>
#include "SFML/System/Time.hpp"
#include "chip8.hpp"
#include "utils.hpp"

bool Options::verbose = false;
bool Options::debug = false;

int main(int argc, char *argv[])
{
    if(argc < 2) {
        error("Too few arguments!");
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
        error("No input file path!");
        return 2;
    }

    sf::RenderWindow window = sf::RenderWindow{ { 640u, 320u}, "CHIP-8"};
    window.setFramerateLimit(60);
    
    sf::Clock clock;
    sf::Time deltaTime;
    sf::Time accuTime;
    sf::Time prevTime;
    sf::Time currTime;

    std::unique_ptr<Chip8> myChip8;
    myChip8 = std::make_unique<Chip8>();
    myChip8->loadFile(inputFileName);

    if (Options::verbose)
        myChip8->getMemory().printProgram();

    if (Options::debug) {
        while (window.isOpen()) {
            sf::Event event = sf::Event{};
            while(window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
                else if (   event.type == sf::Event::KeyReleased
                        &&  event.key.scancode == sf::Keyboard::Scan::Enter) {
                    myChip8->emulateCycle(event);
                    if(myChip8->getDrawFlag())
                        myChip8->drawScreen(window);
                }
            }
        }
    }
    else {
        while (window.isOpen()) {
            sf::Event event = sf::Event{};
            while(window.pollEvent(event)) {
                if(event.type == sf::Event::Closed) {
                    window.close();
                    break;
                }
                else if(event.type == sf::Event::KeyReleased && myChip8->getIsWaitingForKeyboardInput()) {
                    myChip8->loadKeyToV(event);
                }
            }
            deltaTime = clock.restart();

            if(deltaTime > sf::milliseconds(100))
                deltaTime = sf::milliseconds(100);

            accuTime += deltaTime;
            const sf::Time one_sixtieth_of_a_second = sf::microseconds(16670);
            
            // Goal: 500Hz clock speed (500 cycle emulations per second) with 60Hz updates
            // for every 1/60s -> update timers
            for(; accuTime >= one_sixtieth_of_a_second; accuTime -= one_sixtieth_of_a_second) {

                myChip8->updateTimers();
                
                //  60*8 =(approx) 500
                for(size_t i = 0; i < 8 && (!myChip8->getIsWaitingForKeyboardInput()); i++) {
                    myChip8->emulateCycle(event);
                    
                    if(myChip8->getDrawFlag())
                        myChip8->drawScreen(window);
                }
            }
        }
    }
    return 0;
}
