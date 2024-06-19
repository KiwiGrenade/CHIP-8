#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <memory>
#include <array>
#include "SFML/System/Vector2.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include "component.hpp"

class Pixel {
    sf::RectangleShape pix;

public:
    static constexpr float dim = 10.f;
    
    Pixel(const sf::Vector2f& position);

    bool getState();
    void setState(const bool& state);
    sf::RectangleShape& getShape();

    ~Pixel() = default;
};

struct Screen : Component {
    static constexpr unsigned short height = 32;
    static constexpr unsigned short width = 64;
    static constexpr unsigned short nPixels = width * height;

    Screen();
    void clear();

    std::shared_ptr<Pixel> getPixel(const unsigned short& x, const unsigned short& y);
    ~Screen() = default;
    std::array<std::shared_ptr<Pixel>, nPixels> pixels;
};


#endif // DISPLAY_HPP
