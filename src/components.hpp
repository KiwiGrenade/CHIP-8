#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <memory>
#include <array>
#include <fstream>
#include "SFML/System/Vector2.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
struct Component {
    virtual inline void clear() = 0;
};
    /* CHIP-8 has 4KB memory (4096 bytes), from location 0x000 (0) to 0xFFF (4095):
     * + 0x000 (0) to 0x1FF (511) - CHIP-8 interpreter
     * + 0x200 (512) to 0xFFF (4095) - program memory (ETI 660 programs start at 0x600 (1536))*/
struct Memory : Component {
    static constexpr unsigned short size = 4096;
    static constexpr unsigned short programBegin = 512;

    typedef std::array<unsigned char, size> memory_type;
    std::unique_ptr<memory_type> memory;

    Memory(){
        memory = std::make_unique<memory_type>();
        std::fill(memory->begin(), memory->end(), 0);
    }

    inline void clear() { std::fill(memory->begin(), memory->end(), 0); }

    unsigned char& operator[](const std::size_t idx) { return (*memory)[idx]; }
    const unsigned char& operator[](const std::size_t idx) const { return (*memory)[idx]; }

    void loadProgram(std::ifstream& file) {
        for(size_t i = programBegin; !file.eof() && i < size; i++)
            file >> (*memory)[i];
    }

    ~Memory() = default;
};

class Pixel {
    sf::RectangleShape pix;

public:
    static constexpr float dim = 10.f;
    
    Pixel(const sf::Vector2f& position) { 
        pix = sf::RectangleShape(sf::Vector2f(dim, dim));
        pix.setPosition(position);
        setState(false);
    }

    bool getState() { return pix.getFillColor() == sf::Color::White ? true : false; }

    void setState(const bool& state){
        if(state)
            pix.setFillColor(sf::Color::White);
        else
            pix.setFillColor(sf::Color::Black);
    }

    sf::RectangleShape& getShape() { return pix; }

    ~Pixel() = default;
};

class Screen : Component {
public:
    static constexpr unsigned short height = 32;
    static constexpr unsigned short width = 64;
    static constexpr unsigned short nPixels = width * height;

    Screen(){
        pixels = std::make_unique<pixels_type>();

        // set pixel position 
        for(unsigned short i = 0; i < height; i++) {
            for(unsigned short j = 0; j < width; j++) {
                float x = float(j * Pixel::dim);
                float y = float(i * Pixel::dim);

                (*pixels)[i * width + j] = new Pixel(sf::Vector2f(x, y));
            }
        }
    }

    inline void clear() {
        for(auto a : *pixels) {
            a->setState(false);
        }
    }

    inline Pixel* getPixel(const unsigned short& x, const unsigned short& y) {
        return (*pixels)[(y % height) * width + (x % width)];
    }
    
    ~Screen() = default;
private:
    typedef std::array<Pixel*, nPixels> pixels_type;
    typedef std::array<bool, nPixels> board_type;
    std::unique_ptr<pixels_type> pixels;
};

#endif //COMPONENTS_HPP
