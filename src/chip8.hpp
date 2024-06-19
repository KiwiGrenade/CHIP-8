#ifndef CHIP8_HPP
#define CHIP8_HPP

#include <memory>
#include <SFML/Graphics/RenderWindow.hpp>
#include "display.hpp"
#include "memory.hpp"

struct Chip8 {
    bool            drawFlag;
    unsigned short  pc;      // program counter
    unsigned short  opcode;  // current opcode (opcodes are 2 bytes)
    unsigned short  I;       // memory pointer
    unsigned short  sp;      // stack pointer
    unsigned char   V[16];    // 16 * 1 byte registers (VF is carry flag)
    unsigned char   sound_timer;
    unsigned char   delay_timer;
    unsigned short  stack[16];
    unsigned char   key[16];

    std::unique_ptr<Memory> memory;
    std::unique_ptr<Screen> screen;

    Chip8();
    void initialize();
    void loadFile(const std::string& filename);
    void emulateCycle();
    void drawScreen(sf::RenderWindow& window);
};

#endif //CHIP8_HPP
