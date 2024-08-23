#include "Chip8.hpp"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#include <QThread>
#include <QElapsedTimer>

#include "utils.hpp"

uint8_t         Chip8::soundTimer;
QReadWriteLock  Chip8::soundTimerLock;
uint8_t         Chip8::delayTimer;
QReadWriteLock  Chip8::delayTimerLock;

bool        Chip8::isWaitingForKeyboardInput;
bool        Chip8::paused;
bool        Chip8::alive;

Chip8::Chip8() : 
    memory(std::make_shared<Memory>()),
    screen(std::make_shared<Screen>())
    {
    paused = false;
    alive = false;
    std::srand(time(nullptr));
    clear();
}

void Chip8::loadFile(const std::string& fileName) {
    memory->loadFile(fileName);
}

void Chip8::restart() {
    paused = false;
    clear();
    memory->reloadFile();
}

void Chip8::pause() { 
    paused = true;
}

void Chip8::unPause() {
    paused = false;
}

void Chip8::stop() {
    alive = false;
    wait();
}

void Chip8::clear() {
    if(paused)
        return;
    isWaitingForKeyboardInput = false;
    nCycle = 0;
    pc = Memory::programBegin;
    opcode = 0;
    I = 0;
    sp = 0;
    drawFlag = false;

    soundTimerLock.lockForWrite();
    soundTimer = 0;
    soundTimerLock.unlock();
    delayTimerLock.lockForWrite();
    delayTimer = 0;
    delayTimerLock.unlock();

    std::fill(std::begin(stack), std::end(stack), 0);
    std::fill(std::begin(V), std::end(V), 0);
   
    memory->loadFontset();
}

void Chip8::updateTimers() {
    if (paused || !alive)
        return;
    if(soundTimer > 0) {
        soundTimerLock.lockForWrite();
        soundTimer--;
        soundTimerLock.unlock();
    }
    if(delayTimer > 0) {
        delayTimerLock.lockForWrite();
        delayTimer--;
        delayTimerLock.unlock();
    }
}

void Chip8::drawSprite(
    const uint16_t& n,
    const uint16_t& x,
    const uint16_t& y,
    uint8_t& VF) {
    VF = 0;

    for(uint16_t i = 0; i < n; ++i) {

        uint16_t yrow = (V[y] % Screen::yRes_) + i;
        uint8_t row = (*memory)[I+i];

        if(yrow >= Screen::yRes_)
            continue;

        for(uint8_t j = 0; j < 8; ++j) {

            uint16_t xcol = (V[x] % Screen::xRes_) + j;
            bool currbit = (row >> (8-j-1)) & 1;

            if(xcol >= Screen::xRes_)
                continue;

            bool pixel = screen->getPixel(xcol, yrow);
            if(pixel == true && currbit)
                VF = 1;

            Screen::setPixel(xcol, yrow, pixel ^ currbit);
        }
    }
}

void Chip8::emulateCycle() {
    if(paused || !alive || !memory->isFileLoaded())
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

    /*if(Options::verbose)*/
    /*    printData(x, y, n, kk, VF, nnn);*/
    
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
                    if(keysDown.contains(V[x]))
                        pc += 2;
                    break;
                case 0x00A1: // 0xEXA1: Skip next instr. if key with the value of V[X] is NOT pressed
                    if(!keysDown.contains(V[x]))
                        pc += 2;
                    break;
                default:
                    unknownOpcode(opcode);
                    break;
            }
            break;
        case 0xF000:
            switch(opcode & 0x00FF){
                case 0x0007: // 0xFX07: V[X] = delayTimer
                    delayTimerLock.lockForRead();
                    V[x] = delayTimer;
                    delayTimerLock.unlock();
                    break;
                case 0x000A: // 0xFX0A: Wait for a key press, store the value of the key in V[X]
                    isWaitingForKeyboardInput = keysDown.empty();

                    if(isWaitingForKeyboardInput) {
                        pc -= 2;
                    }
                    else {
                        keysDownLock.lockForRead();
                        V[x] = *keysDown.begin();
                        keysDownLock.unlock();
                    }
                    break;
                case 0x0015: // 0xFX15: delayTimer = V[X]
                    delayTimerLock.lockForWrite();
                    delayTimer = V[x];
                    delayTimerLock.unlock();
                    break;
                case 0x0018: // 0xFX18: soundTimer = V[X]
                    soundTimerLock.lockForWrite();
                    soundTimer = V[x];
                    delayTimerLock.unlock();
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
    
    soundTimerLock.lockForRead();
    if(soundTimer == 1) {
        std::cout << "BEEP!\a" << std::endl;
    }
    soundTimerLock.unlock();

    nCycle++;
}

void Chip8::run() {
    if(!memory->isFileLoaded())
        return;
    clear();
    screen->clear();

    paused = false;
    alive = true;
    QElapsedTimer timer;
    uint64_t deltaTime = 0;
    uint64_t accuTime = 0;

    timer.start();
    while(alive) {
        deltaTime = timer.nsecsElapsed();
        timer.restart();

        if(deltaTime > 1000000000)
            deltaTime = 1000000000;

        accuTime += deltaTime;
        for(;accuTime >= 16670000; accuTime -= 16670000) {
            for(size_t i = 0; i < 8; ++i) {
                emulateCycle();
            }
        }
    }
}

void Chip8::unknownOpcode(const uint16_t& opcode) {
    printf("Unknown opcode: 0x%04x\n", opcode);
    exit(2);
}

void Chip8::printData(
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

void Chip8::addKeyDown(const unsigned char& keyVal) {
    keysDownLock.lockForWrite();
    keysDown.insert(keyVal);
    keysDownLock.unlock();
}

void Chip8::removeKeyDown(const unsigned char& keyVal) {
    keysDownLock.lockForWrite();
    keysDown.erase(keyVal);
    keysDownLock.unlock();
}

