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
    
}


