#ifndef CHIP8_HPP
#define CHIP8_HPP

#include "components.hpp"
#include <memory>

struct Chip8 {
    unsigned short pc;
    unsigned short opcode; // current opcode (opcodes are 2 bytes)
    unsigned short I;
    unsigned short sp;
    unsigned char V[16]; // 16 * 1 byte registers (VF is carry flag)

    std::unique_ptr<Memory> memory;

    unsigned char sound_timer;
    unsigned char delay_timer;
    unsigned short stack[16];
    unsigned char key[16];

    std::unique_ptr<Screen> screen;
    Chip8();
    void initialize();
    void loadFile(std::string filename);
    void emulateCycle();
};

#endif //CHIP8_HPP