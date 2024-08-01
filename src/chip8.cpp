#include <SFML/Window/Window.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <iostream>
#include <time.h>
#include <filesystem>
#include "chip8.hpp"
#include "utils.hpp"

Chip8::Chip8() {
    std::srand(time(nullptr));

    nCycle = 0;
    pc = Memory::programBegin;
    sound_timer = 0;
    delay_timer = 0;
    opcode = 0;
    I = 0;
    sp = 0;
    drawFlag = false;

    memory = std::make_unique<Memory>();
    screen = std::make_unique<Screen>();
    
    screen->clear();
    std::fill(std::begin(stack), std::end(stack), 0);
    std::fill(std::begin(V), std::end(V), 0);
    memory->clear();
    memory->loadFontset();
}

void Chip8::drawScreen(sf::RenderWindow& window) {
    for(unsigned short i = 0; i < Screen::height; ++i) {
        for(unsigned short j = 0; j < Screen::width; ++j) {
            std::shared_ptr<Pixel> pixel = screen->getPixel(j, i);
            window.draw(pixel->getShape());
        }
    }
    window.display();
}

void Chip8::loadFile(const std::string& filename) {
    std::filesystem::path filePath{filename};
    if(std::filesystem::exists(filePath) == false) {
        error("File " + filePath.string() + " does not exist!");
    }

    auto length = std::filesystem::file_size(filePath);
    std::ifstream inputFile(filename, std::ios_base::binary);
    if((length == 0) || (inputFile.is_open() == false) || inputFile.bad()) {
        error("Could not open file: " + filePath.string());
    }

    std::vector<unsigned char> buffer(length);
    inputFile.read(reinterpret_cast<char*>(buffer.data()), length);
    inputFile.close();

    memory->loadProgram(buffer);

    inputFile.close();
}

void unknownOpcode(const unsigned short& opcode) {
    printf("Unknown opcode: 0x%04x\n", opcode);
    exit(2);
}

void Chip8::emulateCycle() {
    opcode = memory->getOpcode(pc);
    pc+=2;

    unsigned short  nnn     =    opcode & 0x0FFF;
    unsigned short  n       =    opcode & 0x000F;
    unsigned short  x       =   (opcode & 0x0F00) >> 8;
    unsigned short  y       =   (opcode & 0x00F0) >> 4;
    unsigned short  kk      =    opcode & 0x00FF;
    unsigned char&  VF      =    V[0xF];
    unsigned char   tempVF  =    0;

    drawFlag = false;

    if(Options::verbose) {
        std::cout   << "| cycle # | opcode | x | y | kk | nnn | n | VF | PC | SP" << std::endl
            << " | " << std::dec << nCycle << " | " << std::hex << opcode << " | " << x << " | " << y 
            << " | " << kk << " | " << nnn << " | " << n << " | " << VF << " | " << std::dec << pc-2 << " | " << sp << std::endl;
    }
    
    switch(opcode & 0xF000) { // check first 4 bits
        case 0x0000:
            switch (n) { // check nibble
                case 0x0000: // 0x00E0: Clear screen
                    screen->clear();
                    drawFlag = true;
                    break;
                case 0x000E: // 0x00EE: Return from subroutine
                    --sp;
                    pc = stack[sp];
                    break;
                default:
                    unknownOpcode(opcode);
                    break;
            }
            break;
        case 0x1000: // 0x1NNN: Jump to location NNN
            pc = nnn; break;
        case 0x2000: // 0x2NNN: Call subroutine at NNN
            stack[sp] = pc; // earlier incremented by 2
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
            V[x] = kk; break;
        case 0x7000: // 0x7XKK: V[X] += KK
            V[x] += kk; break;
        case 0x8000:
            switch(n) {
                case 0x0000: // 0x8XY0: V[X] = V[Y]
                    V[x] = V[y]; break;
                case 0x0001: // 0x8XY1: V[X] OR V[Y]
                    V[x] |= V[y]; break;
                case 0x0002: // 0x8XY2: V[X] AND V[Y]
                    V[x] &= V[y]; break;
                case 0x0003: // 0x8XY3: V[X] XOR V[Y]
                    V[x] ^= V[y]; break;
                case 0x0004: // 0x8XY4: V[X] ADD V[Y]
                    tempVF = (V[x] + V[y]) > 255;
                    V[x] = (V[x] + V[y]) & 0x00FF;
                    VF = tempVF;
                    break;
                case 0x0005: // 0x8XY5: V[X] SUB V[Y]
                    tempVF = V[x] >= V[y];
                    V[x] -= V[y];
                    VF = tempVF;
                    break;
                case 0x0006: // 0x8XY6: V[X] = V[X] / 2 
                    VF = V[x] & 1; // check if last bit is 1
                    V[x] = V[x] >> 1;
                    break;
                case 0x0007: // 0x8XY7: V[X] SUBN V[Y]
                    tempVF = V[y] >= V[x];
                    V[x] = (V[y] - V[x]);
                    VF = tempVF;
                    break;
                case 0x000E: // 0x8XYE: V[X] = V[X] * 2 
                    VF = V[x] & 0x8000; // set to most significant bit of
                    V[x] = V[x] << 1; break;
                default:
                    unknownOpcode(opcode); break;
            }
            break;
        case 0x9000: // 0x9XY0: Skip next instr. if V[X] != V[Y]
            if(V[x] != V[y])
                 pc+=2;
            break;
        case 0xA000: // 0xANNN: I = NNN
            I = nnn; break;
        case 0xB000: // 0xBNNN: pc = NNN + V[0]
            pc = nnn + V[0]; break;
        case 0xC000: // 0xCXKK: V[X] = random byte AND KK
            // TODO: Replace rand() with something else
            V[x] = (rand() % 256) & kk; break;
        case 0xD000: // 0xDXYN: Display n-byte sprite starting at memory location I at (V[X], V[Y]), V[F] = collision;
            VF = 0;

            for(unsigned short i = 0; i < n; ++i) {

                unsigned short yrow = V[y] + i;
                unsigned char row = (*memory)[I+i];

                for(unsigned char j = 0; j < 8; ++j) {

                    unsigned short xcol = V[x] + j;
                    bool curr_bit = (row >> (8-j-1)) & 1;

                    if(curr_bit) {
                        std::shared_ptr<Pixel> pixel = screen->getPixel(xcol, yrow);

                        if(pixel->getState() ^ curr_bit) {
                            pixel->setState(curr_bit);
                            VF = 1;
                        }
                    }
                }
            }

            drawFlag = true;
            break;
        case 0xE000:
            switch(kk){
                case 0x009E: // 0xEX9E: Skip next instr. if key with the value of V[X] is pressed
                    if(sf::Keyboard::isKeyPressed(getKey(x)) == true)
                        pc+=2;
                    break;
                case 0x00A1: // 0xEXA1: Skip next instr. if key with the value of V[X] is NOT pressed
                    if(sf::Keyboard::isKeyPressed(getKey(x)) == false)
                        pc+=2;
                    break;
                default:
                    unknownOpcode(opcode); break;
            }
            break;
        case 0xF000:
            switch(opcode & 0x00FF){
                case 0x0007: // 0xFX07: V[X] = delay_timer
                    V[x] = delay_timer; break;
                case 0x000A: // 0xFX0A: Wait for a key press, store the value of the key in V[X]
                    std::cerr << "Not implemented yet!" << std::endl; break;
                case 0x0015: // 0xFX15: delay_timer = V[X]
                    delay_timer = V[x]; break;
                case 0x0018: // 0xFX18: sound_timer = V[X]
                    sound_timer = V[x]; break;
                case 0x001E: // 0xFX1E: I = I + V[X]
                    I = I + V[x]; break;
                case 0x0029: // 0xFX29: I = location_of_sprite_for_digit_V[X]
                    I = 4 * V[x]; break;
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
                default:
                    unknownOpcode(opcode); break;
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
    nCycle++;
}

sf::Keyboard::Key Chip8::getKey(const unsigned char& x) {
    sf::Keyboard::Key key;
    switch(V[x]) {
        case 0x1:
            key = sf::Keyboard::Num1; break;
        case 0x2:
            key = sf::Keyboard::Num2; break;
        case 0x3:
            key = sf::Keyboard::Num3; break;
        case 0x4:
            key = sf::Keyboard::Q; break;
        case 0x5:
            key = sf::Keyboard::W; break;
        case 0x6:
            key = sf::Keyboard::E; break;
        case 0x7:
            key = sf::Keyboard::A; break;
        case 0x8:
            key = sf::Keyboard::S; break;
        case 0x9:
            key = sf::Keyboard::D; break;
        case 0xA:
            key = sf::Keyboard::Z; break;
        case 0xB:
            key = sf::Keyboard::C; break;
        case 0xC:
            key = sf::Keyboard::Num4; break;
        case 0xD:
            key = sf::Keyboard::R; break;
        case 0xE:
            key = sf::Keyboard::F; break;
        case 0xF:
            key = sf::Keyboard::V; break;
        case 0x0:
            key = sf::Keyboard::X; break;
    };
    return key;
}

