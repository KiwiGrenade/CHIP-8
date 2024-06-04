#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

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

//TODO: tidy it later
struct Screen : Component {
    static constexpr unsigned short height = 32;
    static constexpr unsigned short width = 64;
    static constexpr float pixel_dim = 10.f;
    static constexpr unsigned short size = width * height; // 64p wide, 32p high

    const sf::Color OFF = sf::Color::Black;
    const sf::Color ON = sf::Color::White;

    typedef std::array<sf::RectangleShape, size> board_type;
    typedef std::array<bool, size> screen_type;
    std::unique_ptr<screen_type> screen;
    std::unique_ptr<board_type> board;

    Screen(){
        screen = std::make_unique<screen_type>();
        board = std::make_unique<board_type>();
        
        std::fill(board->begin(), board->end(), sf::RectangleShape(sf::Vector2f(pixel_dim, pixel_dim)));
        // turn off pixels
        for(auto pixel : *board) {
            pixel.setFillColor(OFF);
        }

        // for(unsigned char i = 0; i < height)
    }

    inline void clear() {
        std::fill(screen->begin(), screen->end(), false);
        for(auto a : *board) {
            a.setFillColor(ON);
        }
    }

    ~Screen() = default;
};

#endif //COMPONENTS_HPP
