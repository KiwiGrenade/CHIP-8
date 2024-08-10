#include <SFML/Window/Window.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <string>
#include <iostream>
#include <time.h>
#include <filesystem>
#include "chip8.hpp"
#include "utils.hpp"

Chip8::Chip8() {
    std::srand(time(nullptr));

    isWaitingForKeyboardInput_ = false;
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
    VF = 0;

    for(unsigned short i = 0; i < n; ++i) {

        unsigned short yrow = (V_[y] % Screen::height) + i;
        unsigned char row = (*memory_)[I_+i];

        if(yrow >= Screen::height)
            continue;

        for(unsigned char j = 0; j < 8; ++j) {

            unsigned short xcol = (V_[x] % Screen::width) + j;
            bool curr_bit = (row >> (8-j-1)) & 1;

            if(xcol >= Screen::width)
                continue;

            std::shared_ptr<Pixel> pixel = screen_->getPixel(xcol, yrow);
            if(pixel->getState() == true && curr_bit)
                VF = 1;
            pixel->setState(pixel->getState() ^ curr_bit);
        }
    }
}

using namespace sf;

const unsigned char Chip8::getKeyToChar(const sf::Keyboard::Scancode& key) {
    switch (key) {
        case Keyboard::Scan::Num1:
            return 0x1;
        case Keyboard::Scan::Num2:
            return 0x2;
        case Keyboard::Scan::Num3:
            return 0x3;
        case Keyboard::Scan::Q:
            return 0x4;
        case Keyboard::Scan::W:
            return 0x5;
        case Keyboard::Scan::E:
            return 0x6;
        case Keyboard::Scan::A:
            return 0x7;
        case Keyboard::Scan::S:
            return 0x8;
        case Keyboard::Scan::D:
            return 0x9;
        case Keyboard::Scan::Z:
            return 0xA;
        case Keyboard::Scan::C:
            return 0xB;
        case Keyboard::Scan::Num4:
            return 0xC;
        case Keyboard::Scan::R:
            return 0xD;
        case Keyboard::Scan::F:
            return 0xE;
        case Keyboard::Scan::V:
            return 0xF;
        default:
            return 0x0;
    }
}

const sf::Keyboard::Key Chip8::charToKey(const unsigned char& x) {
    switch(V_[x]) {
        case 0x1:
            return Keyboard::Num1;
        case 0x2:
            return Keyboard::Num2;
        case 0x3:
            return Keyboard::Num3;
        case 0x4:
            return Keyboard::Q;
        case 0x5:
            return Keyboard::W;
        case 0x6:
            return Keyboard::E;
        case 0x7:
            return Keyboard::A;
        case 0x8:
            return Keyboard::S;
        case 0x9:
            return Keyboard::D;
        case 0xA:
            return Keyboard::Z;
        case 0xB:
            return Keyboard::C;
        case 0xC:
            return Keyboard::Num4;
        case 0xD:
            return Keyboard::R;
        case 0xE:
            return Keyboard::F;
        case 0xF:
            return Keyboard::V;
        case 0x0:
            return Keyboard::X;
        default:
            return Keyboard::Unknown;
    };
}

void Chip8::printData(
    const unsigned short& x,
    const unsigned short& y,
    const unsigned short& n,
    const unsigned short& kk,
    const unsigned short& VF,
    const unsigned short& nnn) {
    std::cout   << "| cycle # | opcode_ | x | y | kk | nnn | n | VF | pc_ | sp_" << std::endl << std::dec
                << "| " << nCycle_ << " | " << std::hex << opcode_ << " | " << x << " | " << y 
                << " | " << kk << " | " << nnn << " | " << n << " | " << VF << " | " << std::dec << pc_-2 << " | " << sp_ << std::endl;
}

void Chip8::emulateCycle(const sf::Event& event) {
    opcode_ = memory_->getOpcode(pc_);
    pc_ += 2;
    drawFlag_ = false;

    unsigned short  nnn     =    opcode_ & 0x0FFF;
    unsigned short  n       =    opcode_ & 0x000F;
    unsigned short  x       =   (opcode_ & 0x0F00) >> 8;
    unsigned short  y       =   (opcode_ & 0x00F0) >> 4;
    unsigned short  kk      =    opcode_ & 0x00FF;
    unsigned char&  VF      =    V_[0xF];
    unsigned char   tempVF  =    0;

    if(Options::verbose) {
        printData(x, y, n, kk, VF, nnn);
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
            pc_ = nnn;
            break;
        case 0x2000: // 0x2NNN: Call subroutine at NNN
            stack_[sp_] = pc_; // earlier incremented by 2
            ++sp_;
            pc_ = nnn;
            break;
        case 0x3000: // 0x3XKK: Skip next instr. if V_[X] == KK
            if(V_[x] == kk)
                pc_ += 2;
            break;
        case 0x4000: // 0x4XKK: Skip next instr. if V_[X] != KK
            if(V_[x] != kk)
                pc_ += 2;
            break;
        case 0x5000: // 0x5XY0: Skip next instr. if V_[X] == V[Y]
            if(V_[x] == V_[y])
                pc_ += 2;
            break;
        case 0x6000: // 0x6XKK: V_[X] = KK 
            V_[x] = kk;
            break;
        case 0x7000: // 0x7XKK: V_[X] += KK
            V_[x] += kk;
            break;
        case 0x8000:
            switch(n) {
                case 0x0000: // 0x8XY0: V_[X] = V[Y]
                    V_[x] = V_[y];
                    break;
                case 0x0001: // 0x8XY1: V_[X] OR V[Y]
                    tempVF = 0;
                    V_[x] |= V_[y];
                    VF = tempVF;
                    break;
                case 0x0002: // 0x8XY2: V_[X] AND V[Y]
                    tempVF = 0;
                    V_[x] &= V_[y];
                    VF = tempVF;
                    break;
                case 0x0003: // 0x8XY3: V_[X] XOR V[Y]
                    tempVF = 0;
                    V_[x] ^= V_[y];
                    VF = tempVF;
                    break;
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
                    V_[x] = V_[y];
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
                    V_[x] = V_[y];
                    V_[x] <<= 1;
                    VF = tempVF;
                    break;
                default:
                    unknownOpcode(opcode_); break;
            }
            break;
        case 0x9000: // 0x9XY0: Skip next instr. if V_[X] != V[Y]
            if(V_[x] != V_[y])
                 pc_ += 2;
            break;
        case 0xA000: // 0xANNN: I_ = NNN
            I_ = nnn;
            break;
        case 0xB000: // 0xBNNN: pc_ = NNN + V_[0]
            pc_ = nnn + V_[0];
            break;
        case 0xC000: // 0xCXKK: V_[X] = random byte AND KK
            // TODO: Replace rand() with something else
            V_[x] = (rand() % 256) & kk;
            break;
        case 0xD000: // 0xDXYN: Disp_lay n-byte sp_rite starting at memory_ location I_ at (V_[X], V[Y]), V[F] = collision;
            drawSprite(n, x, y, VF);
            drawFlag_ = true;
            break;
        case 0xE000:
            switch(kk){
                case 0x009E: // 0xEX9E: Skip next instr. if key with the value of V_[X] is pressed
                    if(sf::Keyboard::isKeyPressed(charToKey(x)))
                        pc_ += 2;
                    break;
                case 0x00A1: // 0xEXA1: Skip next instr. if key with the value of V_[X] is NOT pressed
                    if(!sf::Keyboard::isKeyPressed(charToKey(x)))
                        pc_ += 2;
                    break;
                default:
                    unknownOpcode(opcode_);
                    break;
            }
            break;
        case 0xF000:
            switch(opcode_ & 0x00FF){
                case 0x0007: // 0xFX07: V_[X] = delay_timer
                    V_[x] = delay_timer_;
                    break;
                case 0x000A: // 0xFX0A: Wait for a key press, store the value of the key in V_[X]
                    if(event.type == sf::Event::KeyReleased) {
                        V_[x] = getKeyToChar(event.key.scancode);
                        isWaitingForKeyboardInput_ = false;
                    }
                    else {
                        pc_ -= 2;
                        isWaitingForKeyboardInput_ = true;
                    }
                    break;
                case 0x0015: // 0xFX15: delay_timer = V_[X]
                    delay_timer_ = V_[x];
                    break;
                case 0x0018: // 0xFX18: sound_timer = V_[X]
                    sound_timer_ = V_[x];
                    break;
                case 0x001E: // 0xFX1E: I_ = I_ + V_[X]
                    I_ = I_ + V_[x];
                    break;
                case 0x0029: // 0xFX29: I_ = location_of_sp_rite_for_digit_V_[X]
                    I_ = 4 * V_[x];
                    break;
                case 0x0033: // 0xFX33: Store BCD representation of V_[X] in memory_ locations I_, I_+1 and I_+2
                    (*memory_)[I_] = V_[x] / 100; // ones
                    (*memory_)[I_+1] = (V_[x] / 10) % 10; // tens
                    (*memory_)[I_+2] = (V_[x] % 100) % 10; // hundreds
                    break;
                case 0x0055: // 0xFX55: Store registers V_[0x0] through V[X] in memory_ starting at location I_
                    for(unsigned char i = 0; i <= x; ++i, ++I_)
                        (*memory_)[I_] = V_[i];
                    break;
                case 0x0065: // 0xFX65: Read registers V_[0x0] through V[X] from memory_ starting at location I_
                    for(unsigned char i = 0; i <= x; ++i, ++I_)
                        V_[i] = (*memory_)[I_];
                    break;
                default:
                    unknownOpcode(opcode_);
                    break;
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

