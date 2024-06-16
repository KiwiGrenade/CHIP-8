#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <string>
#include <iostream>
#include "chip8.hpp"

int main()
{
    /* Game loop example:
     *  7. int main(int argc, char **argv) 
     *  8. {
     *  9.   // Set up render system and register input callbacks
     *  10.   setupGraphics();
     *  11.   setupInput();
     *  12. 
     *  13.   // Initialize the Chip8 system and load the game into the memory  
     *  14.   myChip8.initialize();
     *  15.   myChip8.loadGame("pong");
     *  16. 
     *  17.   // Emulation loop
     *  18.   for(;;)
     *  19.   {
     *  20.     // Emulate one cycle
     *  21.     myChip8.emulateCycle();
     *  22. 
     *  23.     // If the draw flag is set, update the screen
     *  24.     if(myChip8.drawFlag)
     *  25.       drawGraphics();
     *  26. 
     *  27.     // Store key press state (Press and Release)
     *  28.     myChip8.setKeys();	
     *  29.   }
     *  30. 
     *  31.   return 0;
     *  32. }
     * */

    std::string welcomeMessage {"CHIP-8 will be here soon! WORK IN PROGRESS"};
    std::string fontFileName {"../assets/fonts/BigBlueTerm437NerdFont-Regular.ttf"};

    sf::Font font;

    if(font.loadFromFile(fontFileName) == false) {
        exit(2);
    }

    sf::Text text(welcomeMessage, font, 64);
    text.setFillColor(sf::Color::Green);
    sf::RenderWindow window = sf::RenderWindow{ { 640u, 320u}, "CHIP-8"};
    window.setFramerateLimit(144);
    
    Chip8* myChip8 = new Chip8();
    myChip8->initialize();
    myChip8->loadFile("../roms/test/1-chip8-logo.ch8");

    int i = 0;
    while (window.isOpen() && i < 50)
    {
        std::cout << std::dec << "#" << i << " ";
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
        i++;
    }

    return 0;
}
