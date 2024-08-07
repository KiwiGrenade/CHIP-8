#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <array>
#include <vector>
#include "component.hpp"

/* CHIP-8 has 4KB memory (4096 bytes), from location 0x000 (0) to 0xFFF (4095):
     * + 0x000 (0) to 0x1FF (511) - CHIP-8 interpreter
     * + 0x200 (512) to 0xFFF (4095) - program memory (ETI 660 programs start at 0x600 (1536))*/

struct Memory : Component {
    static constexpr unsigned short size = 4096;
    static constexpr unsigned short fontsetSize = 80;
    static constexpr unsigned short programBegin = 512;
    std::array<unsigned char, size> arr{0};
    unsigned short programSize = 0;

    static constexpr unsigned char fontset [fontsetSize] { 
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
    

    Memory() = default;

    void clear();

    void loadProgram(std::vector<unsigned char>& program);

    void printProgram();

    void loadFontset();

    const unsigned short getOpcode(const unsigned short& pc);

    static const unsigned short getSpriteLocation(const unsigned char& digit);

    unsigned char& operator[](const std::size_t idx);
    const unsigned char& operator[](const std::size_t idx) const;

    ~Memory() = default;
};



#endif // MEMORY_HPP
