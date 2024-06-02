#include <cstdio>
#include <fstream>
#include <string>
#include <iostream>
#include "chip8.hpp"

Chip8::Chip8() {
    memory = std::make_unique<Memory>();
    screen = std::make_unique<Screen>();
}

void Chip8::initialize() {
    pc = 0x200;
    opcode = 0;
    I = 0;
    sp = 0;
    screen->clear();
    for(unsigned short a : stack) // clear stack
        a = 0;
    for(unsigned char a : V) //clear registers
        a = 0;
    memory->clear();

    // Load fontset
    // for(int i = 0; i < 80; ++i)
    //     memory[i] = fontset[i];
}

void Chip8::loadFile(std::string filename) {
    std::ifstream file = std::ifstream(filename, std::ifstream::binary);

    if(!file.is_open() || file.bad()) {
        std::cerr << "ERROR: Could not open file: " << filename << '\n';
        exit(2);
    }

    for(size_t i = 512; !file.eof() && i < Memory::size; i++) {
        file >> (*memory)[i];
        std::cout << std::hex << static_cast<unsigned>((*memory)[i]) << std::endl;
    }

    file.close();
}

void Chip8::emulateCycle() {
    opcode = (*memory)[pc] << 8 | (*memory)[pc+1];
    
    // 1010 0000 0000 0000 & 
    // 1111 0000 0000 0000 =
    // 1010 0000 0000 0000
    switch(opcode & 0xF000) { // check first 4 bits
        case 0x0000: // 0000 0000 0000 0000 
            if((opcode & 0x0F00) == 0x0000) {
                switch (opcode & 0x000F) { // check last 4 bits
                case 0x0000: // 0x00E0: Clear screen
                    screen->clear();
                    ++pc;
                break;

                case 0x00EE: // 0x00EE: Return from subroutine
                break;

                default:
                    printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
                }
                break;
            }
            else {

            }
            break; 
        case 0xA000: // ANNN
            I = opcode & 0x0FFF;
            pc += 2;
            break;
        default:
            printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
    }

    if(delay_timer > 0)
        --delay_timer;

    if(sound_timer > 0) {
        if(sound_timer == 1)
            std::cout << "BEEP!\n";
        --sound_timer;
    }
}


