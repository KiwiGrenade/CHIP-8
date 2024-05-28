#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
int main()
{
    std::string welcomeMessage {"CHIP-8 will be here soon!"};
    std::string fontFileName {"../assets/fonts/BigBlueTerm437NerdFont-Regular.ttf"};

    sf::Font font;

    if(font.loadFromFile(fontFileName) == false) {
        exit(2);
    }

    sf::Text text(welcomeMessage, font, 64);
    text.setFillColor(sf::Color::Green);

    auto window = sf::RenderWindow{ { 1920u, 1080u }, welcomeMessage};
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
        window.draw(text);
        window.display();
    }
}
