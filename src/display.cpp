#include "display.hpp"

Pixel::Pixel(const sf::Vector2f& position) { 
    pix = sf::RectangleShape(sf::Vector2f(dim, dim));
    pix.setPosition(position);
    setState(false);
}

bool Pixel::getState() { return pix.getFillColor() == sf::Color::White ? true : false; }

void Pixel::setState(const bool& state) { state ? pix.setFillColor(sf::Color::White) : pix.setFillColor(sf::Color::Black); }

sf::RectangleShape& Pixel::getShape() { return pix; }



Screen::Screen(){
    // set pixel position 
    for(unsigned short i = 0; i < height; ++i) {
        float y = float(i * Pixel::dim);
        for(unsigned short j = 0; j < width; ++j) {
            float x = float(j * Pixel::dim);
            pixels[i * width + j] = std::make_shared<Pixel>(Pixel(sf::Vector2f(x, y)));
        }
    }
}

void Screen::clear() {
    for(auto a : pixels)
        a->setState(false);
}

std::shared_ptr<Pixel> Screen::getPixel(const unsigned short& x, const unsigned short& y) {
    return pixels[(y % height) * width + (x % width)];
}
