#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include "SFML/System/Vector2.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <memory>
#include <array>

struct Component {
    virtual inline void clear() = 0;
};
    /* CHIP-8 has 4KB memory (4096 bytes), from location 0x000 (0) to 0xFFF (4095):
     * + 0x000 (0) to 0x1FF (511) - CHIP-8 interpreter
     * + 0x200 (512) to 0xFFF (4095) - program memory (ETI 660 programs start at 0x600 (1536))*/
struct Memory : Component {
    static constexpr unsigned short size = 4096;
    typedef std::array<unsigned char, size> memory_type;
    std::unique_ptr<memory_type> memory;

    Memory(){
        memory = std::make_unique<memory_type>();
        std::fill(memory->begin(), memory->end(), 0);
    }

    inline void clear() { std::fill(memory->begin(), memory->end(), 0); }

    unsigned char& operator[](const std::size_t idx) { return (*memory)[idx]; }
    const unsigned char& operator[](const std::size_t idx) const { return (*memory)[idx]; }

    ~Memory() = default;
};

class Pixel {
    sf::RectangleShape pix;

public:
    static constexpr float dim = 10.f;
    
    Pixel() { pix = sf::RectangleShape(sf::Vector2f(dim, dim)); }

    inline void ON() { pix.setFillColor(sf::Color::White); }

    inline void OFF() { pix.setFillColor(sf::Color::Black); }

    bool getState() {
        if(pix.getFillColor() == sf::Color::White)
            return true;
        return false;
    }

    sf::RectangleShape& getShape() { return pix; }

    void setPosition(const sf::Vector2f& position) { pix.setPosition(position); }

    ~Pixel() = default;
};

class Screen : Component {
    static constexpr unsigned short height = 32;
    static constexpr unsigned short width = 64;
    static constexpr unsigned short size = width * height;
    typedef std::array<Pixel, size> pixels_type;
    std::unique_ptr<pixels_type> pixels;

public:

    Screen(){
        pixels = std::make_unique<pixels_type>();
        
        std::fill(pixels->begin(), pixels->end(), Pixel());

        // turn off pixels
        for(auto p : *pixels)
            p.ON();

        // set pixel position 
        for(unsigned short i = 0; i < height; i++) {
            for(unsigned short j = 0; j < width; j++) {
                float x = float(j * Pixel::dim);
                float y = float(i * Pixel::dim);
                (*pixels)[i * width + j].setPosition(sf::Vector2f(x, y));
            }
        }
    }

    pixels_type& getPixels() { return *pixels; }

    inline void clear() {
        for(auto a : *pixels) {
            a.OFF();
        }
    }

    inline Pixel& getPixel(const unsigned short& x, const unsigned short& y) { return (*pixels)[y * width + x]; }
    
    ~Screen() = default;
};

#endif //COMPONENTS_HPP
