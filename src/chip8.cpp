#include <SFML/Window/Window.hpp>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <iostream>
#include <time.h>
#include "chip8.hpp"

Chip8::Chip8() {
    memory = std::make_unique<Memory>();
    screen = std::make_unique<Screen>();
    std::srand(time(nullptr));
}

void Chip8::drawScreen(sf::RenderWindow& window) {
    for(unsigned short i = 0; i < Screen::height; i++) {
        for(unsigned short j = 0; j < Screen::width; j++) {
            float x = float(j * Pixel::dim);
            float y = float(i * Pixel::dim);
            Pixel* pixel = screen->getPixel(x, y);
            window.draw(pixel->getShape());
        }
    }
    window.display();
}

void Chip8::initialize() {
    pc = Memory::programBegin;
    opcode = 0;
    I = 0;
    sp = 0;
    drawFlag = 0;

    screen->clear();
    for(unsigned short a : stack) // clear stack
        a = 0;
    for(unsigned char a : V) //clear registers
        a = 0;
    memory->clear();

    // Load fontset
    for(unsigned short i = 0; i < 80; ++i)
        (*memory)[i] = fontset[i];
}

void Chip8::loadFile(std::string filename) {
    std::ifstream file = std::ifstream(filename, std::ifstream::binary);

    if(!file.is_open() || file.bad()) {
        std::cerr << "ERROR: Could not open file: " << filename << '\n';
        exit(2);
    }

    memory->loadProgram(file);

    file.close();
}

void unknownOpcode(const unsigned short& opcode) {
    printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
    exit(2);
}

void Chip8::emulateCycle() {
    drawFlag = false;
    opcode = (*memory)[pc] << 8 | (*memory)[pc+1];
    pc+=2;

    unsigned short nnn =    opcode & 0x0FFF;
    unsigned char n    =    opcode & 0x000F;
    unsigned short x    =   (opcode & 0x0F00) >> 8;
    unsigned short y    =   (opcode & 0x00F0) >> 4;
    unsigned char kk   =    opcode & 0x00FE;
    unsigned char& VF = V[0xF];

    std::cout << "Opcode: " << std::hex << opcode << std::endl\
              << "x:      " << x << std::endl
              << "y:      " << y << std::endl;


    switch(opcode & 0xF000) { // check first 4 bits
        case 0x0000:
            switch (n) { // check nibble
            case 0x0000: // 0x00E0: Clear screen
                screen->clear();
                break;
            case 0x000E: // 0x00EE: Return from subroutine
                pc = stack[sp];
                --sp;
                break;
            default:
                unknownOpcode(opcode);
                break;
            }
            break;
        case 0x1000: // 0x1NNN: Jump to location NNN
            pc = nnn;
            break;
        case 0x2000: // 0x2NNN: Call subroutine at NNN
            stack[sp] = pc;
            ++sp;
            pc = nnn;
            break;
        case 0x3000: // 0x3XKK: Skip next instr. if V[X] == KK
            if(V[x] == kk)
                pc+=2;
            break;
        case 0x4000: // 0x4XKK: Skip next instr. if V[X] != KK
            if(V[x] != kk)
                pc+=2;
            break;
        case 0x5000: // 0x5XY0: Skip next instr. if V[X] == V[Y]
            if(V[x] == V[y])
                 pc+=2;
            break;
        case 0x6000: // 0x6XKK: V[X] = KK 
            V[x] = kk;
            break;
        case 0x7000: // 0x7XKK: V[X] += KK
            V[x] += kk;
            break;
        case 0x8000:
            switch(n) {
                case 0x0000: // 0x8XY0: V[X] = V[Y]
                    V[x] = V[y];
                    break;
                case 0x0001: // 0x8XY1: V[X] OR V[Y]
                    V[x] |= V[y];
                    break;
                case 0x0002: // 0x8XY2: V[X] AND V[Y]
                    V[x] &= V[y];
                    break;
                case 0x0003: // 0x8XY3: V[X] XOR V[Y]
                    V[x] ^= V[y];
                    break;
                case 0x0004: // 0x8XY4: V[X] ADD V[Y]
                    // if(V[y] > (0xFF - V[x]))
                    //     VF = 1;
                    // else
                    //     VF = 0;
                    // V[x] += V[y];
                    VF = (V[x] + V[y]) > 255;
                    V[x] = (V[x] + V[y]) & 0x00FF; // store only lowest 8 bits
                    break;
                case 0x0005: // 0x8XY5: V[X] SUB V[Y]
                    VF = V[x] > V[y];
                    V[x] = (V[x] - V[y]) & 0x00FF;
                    break;
                case 0x0006: // 0x8XY6: V[X] = V[X] / 2 
                    VF = V[x] & 0x0001; // check if last bit is 1
                    V[x] = V[x] >> 1;
                    break;
                case 0x0007: // 0x8XY7: V[X] SUBN V[Y]
                    VF = V[y] > V[x];
                    V[x] = (V[y] - V[x]) & 0x00FF;
                    break;
                case 0x000E: // 0x8XYE: V[X] = V[X] * 2 
                    VF = V[x] & 0x8000; // set to most significant bit of
                    V[x] = V[x] << 1;
                    break;
            }
            break;
        case 0x9000: // 0x9XY0: Skip next instr. if V[X] != V[Y]
            if(V[x] != V[y])
                 pc+=2;
            break;
        case 0xA000: // 0xANNN: I = NNN
            I = nnn;
            break;
        case 0xB000: // 0xBNNN: pc = NNN + V[0]
            pc = nnn + V[0];
            break;
        case 0xC000: // 0xCXKK: V[X] = random byte AND KK
            // TODO: Replace rand() with something else
            V[x] = (rand() % 256) & kk;
            break;
        case 0xD000: // 0xDXYN: Display n-byte sprite starting at memory location I at (V[X], V[Y]), V[F] = collision;
            // std::cout << "x = " << x << ", y = " << y << std::endl;
            VF = 0;
            // std::cout << "Drawing!" << std::endl;
            for(unsigned char i = 0; i < n; i++) {
                unsigned short yrow = y + i;
                unsigned char row = (*memory)[I+i];
                for(unsigned char j = 0; j < 8; j++) {
                    unsigned short xline = x + j;
                    bool curr_bit = (row >> (7-j)) & 1;
                    Pixel* pixel = screen->getPixel(xline, yrow); // x: 31, 33 and 63 do not work! pixels aren't showing. Why?
                    // std::cout << "x = " << xline << ", y = " << yrow << std::endl;
                    if(pixel->getState() && !curr_bit) {
                        VF = 1;
                    }
                    pixel->setState(curr_bit);
                }
            }
            drawFlag = true;
            break;
        case 0xE000:
            switch(kk){
                case 0x009E: // 0xEX9E: Skip next instr. if key with the value of V[X] is pressed
                    // TO BO IMPLEMENTED!!!!
                    std::cout << "Not implemented yet!" << std::endl;
                    break;
                case 0x00A1: // 0xEXA1: Skip next instr. if key with the value of V[X] is NOT pressed
                    // TO BO IMPLEMENTED!!!!
                    std::cout << "Not implemented yet!" << std::endl;
                    break;
                default:
                    unknownOpcode(opcode);
                    break;
            }
            break;
        case 0xF000:
            switch(kk){
                case 0x0007: // 0xFX07: V[X] = delay_timer
                    V[x] = delay_timer;
                    break;
                case 0x000A: // 0xFX0A: Wait for a key press, store the value of the key in V[X]
                    std::cout << "Not implemented yet!" << std::endl;
                    break;
                case 0x0015: // 0xFX15: delay_timer = V[X]
                    delay_timer = V[x];
                    break;
                case 0x0018: // 0xFX18: sound_timer = V[X]
                    sound_timer = V[x];
                    break;
                case 0x001E: // 0xFX1E: I = I + V[X]
                    I = I + V[x];
                    break;
                case 0x0029: // 0xFX29: I = location_of_sprite_for_digit_V[X]
                    std::cout << "Not implemented yet!" << std::endl;
                    break;
                case 0x0033: // 0xFX33: Store BCD representation of V[X] in memory locations I, I+1 and I+2
                    (*memory)[I] = V[x] / 100; // ones
                    (*memory)[I+1] = (V[x] / 10) % 10; // tens
                    (*memory)[I+2] = (V[x] % 100) % 10; // hundreds
                    break;
                case 0x0055: // 0xFX55: Store registers V[0x0] through V[X] in memory starting at location I
                    for(unsigned char i = 0; i <= (x); ++i)
                        (*memory)[I+i] = V[i];
                    break;
                case 0x0065: // 0xFX65: Read registers V[0x0] through V[X] from memory starting at location I
                    for(unsigned char i = 0; i <= (x); ++i)
                        V[i] = (*memory)[I+i];
                    break;
            }
            break;
        default:
            unknownOpcode(opcode);
            break;
    }

    if(delay_timer > 0)
        --delay_timer;

    if(sound_timer > 0) {
        if(sound_timer == 1)
            std::cout << "BEEP!\n";
        --sound_timer;
    }
}


