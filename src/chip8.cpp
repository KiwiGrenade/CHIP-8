#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <string>
#include <iostream>
#include <time.h>
#include <filesystem>
#include <QThread>
#include "chip8.hpp"
#include "utils.hpp"

namespace chip8 {

size_t      nCycle;
bool        drawFlag;
uint16_t    lastX;
uint16_t    pc;      // program counter
uint16_t    opcode;  // current opcode (opcodes are 2 bytes)
uint16_t    I;       // memory pointer
uint16_t    sp;      // stack pointer
uint8_t     V[16];    // 16 * 1 byte registers (VF is carry flag)
uint8_t     soundtimer;
uint8_t     delaytimer;
uint16_t    stack[16];
uint8_t     key[16];
bool        isWaitingForKeyboardInput;
bool        isRunning;

std::filesystem::path pathToROM;

std::unique_ptr<Memory> memory;
std::unique_ptr<Screen> screen;

Memory& getMemory() { return *memory; }
Screen& getScreen() { return *screen; }
bool getDrawFlag() { return drawFlag; }
bool getIsWaitingForKeyboardInput() { return isWaitingForKeyboardInput; }
uint8_t getDelayTimer() { return delaytimer; }
uint8_t getSoundTimer() { return soundtimer; }
void start() { isRunning = true; }

void stop() {
    if(!isRunning)
        return;

    isRunning = false;
}

void clear() {
    std::srand(time(nullptr));

    isWaitingForKeyboardInput = false;
    nCycle = 0;
    pc = Memory::programBegin;
    soundtimer = 0;
    delaytimer = 0;
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
    std::cout << std::endl;
}

/*void drawScreen(MainWindow& window) {*/
/*    for(uint16_t i = 0; i < Screen::height_; ++i) {*/
/*        for(uint16_t j = 0; j < Screen::width_; ++j) {*/
/*            std::sharedptr<Pixel> pixel = screen->getPixel(j, i);*/
/*            window.draw(pixel->getShape());*/
/*        }*/
/*    }*/
/*    window.display();*/
/*}*/

void loadFile(const std::string& filename) {
    pathToROM = filename;
    if(std::filesystem::exists(pathToROM) == false) {
        error("File " + pathToROM.string() + " does not exist!");
    }

    auto length = std::filesystem::file_size(pathToROM);
    std::ifstream inputFile(filename, std::ios_base::binary);
    if((length == 0) || (inputFile.is_open() == false) || inputFile.bad()) {
        error("Could not open file: " + pathToROM.string());
    }

    std::vector<uint8_t> buffer(length);
    inputFile.read(reinterpret_cast<char*>(buffer.data()), length);
    inputFile.close();

    memory->loadProgram(buffer);

    inputFile.close();
}

void updateTimers() {
    if(delaytimer > 0)
        delaytimer--;
    if(soundtimer > 0)
        soundtimer--;
}

void emulateCycle() {
    if(!isRunning)
        return;
    opcode = memory->getOpcode(pc);
    pc += 2;
    drawFlag = false;

    uint16_t  nnn     =    opcode & 0x0FFF;
    uint16_t  n       =    opcode & 0x000F;
    uint16_t  x       =   (opcode & 0x0F00) >> 8;
    uint16_t  y       =   (opcode & 0x00F0) >> 4;
    uint16_t  kk      =    opcode & 0x00FF;
    uint8_t&  VF      =    V[0xF];
    uint8_t   tempVF  =    0;
    lastX = x;

    if(Options::verbose)
        printData(x, y, n, kk, VF, nnn);
    
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
            pc = nnn;
            break;
        case 0x2000: // 0x2NNN: Call subroutine at NNN
            stack[sp] = pc; // earlier incremented by 2
            ++sp;
            pc = nnn;
            break;
        case 0x3000: // 0x3XKK: Skip next instr. if V[X] == KK
            if(V[x] == kk)
                pc += 2;
            break;
        case 0x4000: // 0x4XKK: Skip next instr. if V[X] != KK
            if(V[x] != kk)
                pc += 2;
            break;
        case 0x5000: // 0x5XY0: Skip next instr. if V[X] == V[Y]
            if(V[x] == V[y])
                pc += 2;
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
                    tempVF = 0;
                    V[x] |= V[y];
                    VF = tempVF;
                    break;
                case 0x0002: // 0x8XY2: V[X] AND V[Y]
                    tempVF = 0;
                    V[x] &= V[y];
                    VF = tempVF;
                    break;
                case 0x0003: // 0x8XY3: V[X] XOR V[Y]
                    tempVF = 0;
                    V[x] ^= V[y];
                    VF = tempVF;
                    break;
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
                    tempVF = V[x] & 1; // check if last bit is 1
                    V[x] = V[y];
                    V[x] >>= 1;
                    VF = tempVF;
                    break;
                case 0x0007: // 0x8XY7: V[X] SUBN V[Y]
                    tempVF = V[y] >= V[x];
                    V[x] = (V[y] - V[x]);
                    VF = tempVF;
                    break;
                case 0x000E: // 0x8XYE: V[X] = V[X] * 2 
                    tempVF = V[x] >> 7; // set to most significant bit of
                    V[x] = V[y];
                    V[x] <<= 1;
                    VF = tempVF;
                    break;
                default:
                    unknownOpcode(opcode); break;
            }
            break;
        case 0x9000: // 0x9XY0: Skip next instr. if V[X] != V[Y]
            if(V[x] != V[y])
                 pc += 2;
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
            drawSprite(n, x, y, VF);
            drawFlag = true;
            break;
        case 0xE000:
            switch(kk){
                case 0x009E: // 0xEX9E: Skip next instr. if key with the value of V[X] is pressed
                    /*if(sf::Keyboard::isKeyPressed(charToKey(x)))*/
                        pc += 2;
                    break;
                case 0x00A1: // 0xEXA1: Skip next instr. if key with the value of V[X] is NOT pressed
                    /*if(!sf::Keyboard::isKeyPressed(charToKey(x)))*/
                        pc += 2;
                    break;
                default:
                    unknownOpcode(opcode);
                    break;
            }
            break;
        case 0xF000:
            switch(opcode & 0x00FF){
                case 0x0007: // 0xFX07: V[X] = delaytimer
                    V[x] = delaytimer;
                    break;
                case 0x000A: // 0xFX0A: Wait for a key press, store the value of the key in V[X]
                    pc -= 2;
                    isWaitingForKeyboardInput = true;
                    break;
                case 0x0015: // 0xFX15: delaytimer = V[X]
                    delaytimer = V[x];
                    break;
                case 0x0018: // 0xFX18: soundtimer = V[X]
                    soundtimer = V[x];
                    break;
                case 0x001E: // 0xFX1E: I = I + V[X]
                    I = I + V[x];
                    break;
                case 0x0029: // 0xFX29: I = locationofspritefordigitV[X]
                    I = 4 * V[x];
                    break;
                case 0x0033: // 0xFX33: Store BCD representation of V[X] in memory locations I, I+1 and I+2
                    (*memory)[I] = V[x] / 100; // ones
                    (*memory)[I+1] = (V[x] / 10) % 10; // tens
                    (*memory)[I+2] = (V[x] % 100) % 10; // hundreds
                    break;
                case 0x0055: // 0xFX55: Store registers V[0x0] through V[X] in memory starting at location I
                    for(uint8_t i = 0; i <= x; ++i, ++I)
                        (*memory)[I] = V[i];
                    break;
                case 0x0065: // 0xFX65: Read registers V[0x0] through V[X] from memory starting at location I
                    for(uint8_t i = 0; i <= x; ++i, ++I)
                        V[i] = (*memory)[I];
                    break;
                default:
                    unknownOpcode(opcode);
                    break;
            }
            break;
        default:
            unknownOpcode(opcode);
            break; 
    }

    if(soundtimer == 1) {
        std::cout << "BEEP!\a" << std::endl;
    }
    nCycle++;
}


namespace {
void unknownOpcode(const uint16_t& opcode) {
    printf("Unknown opcode: 0x%04x\n", opcode);
    exit(2);
}

void drawSprite(
    const uint16_t n,
    const uint16_t x,
    const uint16_t y,
    uint8_t &VF) {
    VF = 0;

    for(uint16_t i = 0; i < n; ++i) {

        uint16_t yrow = (V[y] % Screen::height_) + i;
        uint8_t row = (*memory)[I+i];

        if(yrow >= Screen::height_)
            continue;

        for(uint8_t j = 0; j < 8; ++j) {

            uint16_t xcol = (V[x] % Screen::width_) + j;
            bool currbit = (row >> (8-j-1)) & 1;

            if(xcol >= Screen::width_)
                continue;

            std::shared_ptr<Pixel> pixel = screen->getPixel(xcol, yrow);
            if(pixel->getState() == true && currbit)
                VF = 1;
            pixel->setState(pixel->getState() ^ currbit);
        }
    }
}

void printData(
    const uint16_t& x,
    const uint16_t& y,
    const uint16_t& n,
    const uint16_t& kk,
    const uint16_t& VF,
    const uint16_t& nnn) {
    std::cout   << "| cycle # | opcode | x | y | kk | nnn | n | VF | pc | sp" << std::endl << std::dec
                << "| " << nCycle << " | " << std::hex << opcode << " | " << x << " | " << y 
                << " | " << kk << " | " << nnn << " | " << n << " | " << VF << " | " << std::dec << pc-2 << " | " << sp << std::endl;
}

} // private namespace
} // namespace chip8

