#include <SFML/Graphics.hpp>
#include <string>
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

    Chip8 myChip8;
    myChip8.initialize();
    myChip8.loadFile("../chip8-test-rom/test_opcode.ch8");

    // while(true) {}

    std::string welcomeMessage {"CHIP-8 will be here soon! WORK IN PROGRESS"};
    std::string fontFileName {"../assets/fonts/BigBlueTerm437NerdFont-Regular.ttf"};

    sf::Font font;

    if(font.loadFromFile(fontFileName) == false) {
        exit(2);
    }

    sf::Text text(welcomeMessage, font, 64);
    text.setFillColor(sf::Color::Green);

    auto window = sf::RenderWindow{ { 640u, 320u}, welcomeMessage};
    window.setFramerateLimit(144);

    while (window.isOpen())
    {
        for (auto event = sf::Event{}; window.pollEvent(event);)
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        window.clear();
        // window.draw(text);
        for(sf::RectangleShape pixel : myChip8.screen->screen) {
            window.draw(pixel);
        }
        // window.draw((*myChip8.screen->board)[0]);
        window.display();
    }
}
