#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <array>
#include <fstream>
#include <memory>
#include <iostream>
#include "SFML/System/Vector2.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
struct Component {
    virtual void clear() = 0;
};
    /* CHIP-8 has 4KB memory (4096 bytes), from location 0x000 (0) to 0xFFF (4095):
     * + 0x000 (0) to 0x1FF (511) - CHIP-8 interpreter
     * + 0x200 (512) to 0xFFF (4095) - program memory (ETI 660 programs start at 0x600 (1536))*/
struct Memory : Component {
    static constexpr unsigned short size = 4096;
    static constexpr unsigned short programBegin = 512;

    unsigned char fontset [80] { 
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    
    std::array<unsigned char, size> arr{0};

    Memory() = default;

    void clear() { std::fill(arr.begin(), arr.end(), 0); }

    void loadProgram(std::ifstream& file) {
        for(size_t i = programBegin; !file.eof() && i < size; ++i) {
            file >> arr[i];
            unsigned short byte = arr[i];
            std::cout << std::hex << byte << std::endl;
        }
    }

    void loadFontset() {
        for(unsigned short i ; i < 80; ++i)
            arr[i] = fontset[i];
    }

    const unsigned short getOpcode(const unsigned short& pc) {
        return arr[pc] << 8 | arr[pc+1];
    }

    unsigned char& operator[](const std::size_t idx) { return arr[idx]; }
    const unsigned char& operator[](const std::size_t idx) const { return arr[idx]; }

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

    void setState(const bool& state) { state ? pix.setFillColor(sf::Color::White) : pix.setFillColor(sf::Color::Black); }

    sf::RectangleShape& getShape() { return pix; }

    ~Pixel() = default;
};

class Screen : Component {
public:
    static constexpr unsigned short height = 32;
    static constexpr unsigned short width = 64;
    static constexpr unsigned short nPixels = width * height;

    Screen(){
        // set pixel position 
        for(unsigned short i = 0; i < height; ++i) {
            for(unsigned short j = 0; j < width; ++j) {
                float x = float(j * Pixel::dim);
                float y = float(i * Pixel::dim);

                pixels[i * width + j] = std::make_shared<Pixel>(Pixel(sf::Vector2f(x, y)));
            }
        }
    }

    void clear() {
        for(auto a : pixels) {
            a->setState(false);
        }
    }

    std::shared_ptr<Pixel> getPixel(const unsigned short& x, const unsigned short& y) {
        return pixels[(y % height) * width + (x % width)];
    }
    
    ~Screen() = default;
private:
    std::array<std::shared_ptr<Pixel>, nPixels> pixels;
};

#endif //COMPONENTS_HPP
