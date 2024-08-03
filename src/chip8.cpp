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

bool Chip8::pause_;

Chip8::Chip8() {
    std::srand(time(nullptr));

    nCycle_ = 0;
    pc_ = Memory::programBegin;
    sound_timer_ = 0;
    delay_timer_ = 0;
    opcode_ = 0;
    I_ = 0;
    sp_ = 0;
    drawFlag_ = false;

    memory_ = std::make_unique<Memory>();
    screen_ = std::make_unique<Screen>();
    
    screen_->clear();
    std::fill(std::begin(stack_), std::end(stack_), 0);
    std::fill(std::begin(V_), std::end(V_), 0);
    memory_->clear();
    memory_->loadFontset();
}

Memory& Chip8::getMemory() {
    return *memory_;
}

Screen& Chip8::getScreen() {
    return *screen_;
}

bool Chip8::getDrawFlag() {
    return drawFlag_;
}

void Chip8::drawScreen(sf::RenderWindow& window) {
    for(unsigned short i = 0; i < Screen::height; ++i) {
        for(unsigned short j = 0; j < Screen::width; ++j) {
            std::shared_ptr<Pixel> pixel = screen_->getPixel(j, i);
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

    memory_->loadProgram(buffer);

    inputFile.close();
}

void unknownOpcode(const unsigned short& opcode) {
    printf("Unknown opcode_: 0x%04x\n", opcode);
    exit(2);
}

void Chip8::drawSprite(
    const unsigned short n,
    const unsigned short x,
    const unsigned short y,
    unsigned char &VF) {
    for(unsigned short i = 0; i < n; ++i) {

        unsigned short yrow = V_[y] + i;
        unsigned char row = (*memory_)[I_+i];

        for(unsigned char j = 0; j < 8; ++j) {

            unsigned short xcol = V_[x] + j;
            bool curr_bit = (row >> (8-j-1)) & 1;

            if(curr_bit) {
                std::shared_ptr<Pixel> pixel = screen_->getPixel(xcol, yrow);

                if(pixel->getState() ^ curr_bit) {
                    pixel->setState(curr_bit);
                    VF = 1;
                }
            }
        }
    }
}

void Chip8::emulateCycle() {
    opcode_ = memory_->getOpcode(pc_);
    pc_+=2;

    unsigned short  nnn     =    opcode_ & 0x0FFF;
    unsigned short  n       =    opcode_ & 0x000F;
    unsigned short  x       =   (opcode_ & 0x0F00) >> 8;
    unsigned short  y       =   (opcode_ & 0x00F0) >> 4;
    unsigned short  kk      =    opcode_ & 0x00FF;
    unsigned char&  VF      =    V_[0xF];
    unsigned char   tempVF  =    0;

    drawFlag_ = false;

    if(Options::verbose) {
        std::cout   << "| cycle # | opcode_ | x | y | kk | nnn | n | VF | pc_ | sp_" << std::endl
            << " | " << std::dec << nCycle_ << " | " << std::hex << opcode_ << " | " << x << " | " << y 
            << " | " << kk << " | " << nnn << " | " << n << " | " << VF << " | " << std::dec << pc_-2 << " | " << sp_ << std::endl;
    }
    
    switch(opcode_ & 0xF000) { // check first 4 bits
        case 0x0000:
            switch (n) { // check nibble
                case 0x0000: // 0x00E0: Clear screen_
                    screen_->clear();
                    drawFlag_ = true;
                    break;
                case 0x000E: // 0x00EE: Return from subroutine
                    --sp_;
                    pc_ = stack_[sp_];
                    break;
                default:
                    unknownOpcode(opcode_);
                    break;
            }
            break;
        case 0x1000: // 0x1NNN: Jump to location NNN
            pc_ = nnn; break;
        case 0x2000: // 0x2NNN: Call subroutine at NNN
            stack_[sp_] = pc_; // earlier incremented by 2
            ++sp_;
            pc_ = nnn;
            break;
        case 0x3000: // 0x3XKK: Skip next instr. if V_[X] == KK
            if(V_[x] == kk)
                pc_+=2;
            break;
        case 0x4000: // 0x4XKK: Skip next instr. if V_[X] != KK
            if(V_[x] != kk)
                pc_+=2;
            break;
        case 0x5000: // 0x5XY0: Skip next instr. if V_[X] == V[Y]
            if(V_[x] == V_[y])
                 pc_+=2;
            break;
        case 0x6000: // 0x6XKK: V_[X] = KK 
            V_[x] = kk; break;
        case 0x7000: // 0x7XKK: V_[X] += KK
            V_[x] += kk; break;
        case 0x8000:
            switch(n) {
                case 0x0000: // 0x8XY0: V_[X] = V[Y]
                    V_[x] = V_[y]; break;
                case 0x0001: // 0x8XY1: V_[X] OR V[Y]
                    V_[x] |= V_[y]; break;
                case 0x0002: // 0x8XY2: V_[X] AND V[Y]
                    V_[x] &= V_[y]; break;
                case 0x0003: // 0x8XY3: V_[X] XOR V[Y]
                    V_[x] ^= V_[y]; break;
                case 0x0004: // 0x8XY4: V_[X] ADD V[Y]
                    tempVF = (V_[x] + V_[y]) > 255;
                    V_[x] = (V_[x] + V_[y]) & 0x00FF;
                    VF = tempVF;
                    break;
                case 0x0005: // 0x8XY5: V_[X] SUB V[Y]
                    tempVF = V_[x] >= V_[y];
                    V_[x] -= V_[y];
                    VF = tempVF;
                    break;
                case 0x0006: // 0x8XY6: V_[X] = V[X] / 2 
                    tempVF = V_[x] & 1; // check if last bit is 1
                    V_[x] >>= 1;
                    VF = tempVF;
                    break;
                case 0x0007: // 0x8XY7: V_[X] SUBN V[Y]
                    tempVF = V_[y] >= V_[x];
                    V_[x] = (V_[y] - V_[x]);
                    VF = tempVF;
                    break;
                case 0x000E: // 0x8XYE: V_[X] = V[X] * 2 
                    tempVF = V_[x] >> 7; // set to most significant bit of
                    V_[x] <<= 1;
                    VF = tempVF;
                    break;
                default:
                    unknownOpcode(opcode_); break;
            }
            break;
        case 0x9000: // 0x9XY0: Skip next instr. if V_[X] != V[Y]
            if(V_[x] != V_[y])
                 pc_+=2;
            break;
        case 0xA000: // 0xANNN: I_ = NNN
            I_ = nnn; break;
        case 0xB000: // 0xBNNN: pc_ = NNN + V_[0]
            pc_ = nnn + V_[0]; break;
        case 0xC000: // 0xCXKK: V_[X] = random byte AND KK
            // TODO: Replace rand() with something else
            V_[x] = (rand() % 256) & kk; break;
        case 0xD000: // 0xDXYN: Disp_lay n-byte sp_rite starting at memory_ location I_ at (V_[X], V[Y]), V[F] = collision;
            VF = 0;
            drawSprite(n, x, y, VF);
            drawFlag_ = true;
            break;
        case 0xE000:
            switch(kk){
                case 0x009E: // 0xEX9E: Skip next instr. if key with the value of V_[X] is pressed
                    if(sf::Keyboard::isKeyPressed(getKey(x)) == true)
                        pc_+=2;
                    break;
                case 0x00A1: // 0xEXA1: Skip next instr. if key with the value of V_[X] is NOT pressed
                    if(sf::Keyboard::isKeyPressed(getKey(x)) == false)
                        pc_+=2;
                    break;
                default:
                    unknownOpcode(opcode_); break;
            }
            break;
        case 0xF000:
            switch(opcode_ & 0x00FF){
                case 0x0007: // 0xFX07: V_[X] = delay_timer
                    V_[x] = delay_timer_; break;
                case 0x000A: // 0xFX0A: Wait for a key press, store the value of the key in V_[X]
                    pause_ = true;
                case 0x0015: // 0xFX15: delay_timer = V_[X]
                    delay_timer_ = V_[x]; break;
                case 0x0018: // 0xFX18: sound_timer = V_[X]
                    sound_timer_ = V_[x]; break;
                case 0x001E: // 0xFX1E: I_ = I_ + V_[X]
                    I_ = I_ + V_[x]; break;
                case 0x0029: // 0xFX29: I_ = location_of_sp_rite_for_digit_V_[X]
                    I_ = 4 * V_[x]; break;
                case 0x0033: // 0xFX33: Store BCD representation of V_[X] in memory_ locations I_, I_+1 and I_+2
                    (*memory_)[I_] = V_[x] / 100; // ones
                    (*memory_)[I_+1] = (V_[x] / 10) % 10; // tens
                    (*memory_)[I_+2] = (V_[x] % 100) % 10; // hundreds
                    break;
                case 0x0055: // 0xFX55: Store registers V_[0x0] through V[X] in memory_ starting at location I_
                    for(unsigned char i = 0; i <= x; ++i)
                        (*memory_)[I_ + i] = V_[i];
                    break;
                case 0x0065: // 0xFX65: Read registers V_[0x0] through V[X] from memory_ starting at location I_
                    for(unsigned char i = 0; i <= x; ++i)
                        V_[i] = (*memory_)[I_ + i];
                    break;
                default:
                    unknownOpcode(opcode_); break;
            }
            break;
        default:
            unknownOpcode(opcode_);
            break; 
    }

    if(delay_timer_ > 0)
        --delay_timer_;

    if(sound_timer_ > 0) {
        if(sound_timer_ == 1)
            std::cout << "BEEP!\n";
        --sound_timer_;
    }
    nCycle_++;
}

sf::Keyboard::Key Chip8::getKey(const unsigned char& x) {
    sf::Keyboard::Key key;
    switch(V_[x]) {
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

