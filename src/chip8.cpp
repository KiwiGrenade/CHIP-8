#include <cstdio>
#include <cstdlib>
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

void unknownOpcode(const unsigned short& opcode) {
    printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
    exit(2);
}

void Chip8::emulateCycle() {
    opcode = (*memory)[pc] << 8 | (*memory)[pc+1];
    
    // 1010 0000 0000 0000 & 
    // 1111 0000 0000 0000 =
    // 1010 0000 0000 0000
    switch(opcode & 0xF000) { // check first 4 bits
        case 0x0000: // 0000 0000 0000 0000 
            switch (opcode & 0x000F) { // check last 4 bits
            case 0x0000: // 0x00E0: Clear screen
                screen->clear();
                pc+=2;
                break;
            case 0x00EE: // 0x00EE: Return from subroutine
                pc = stack[sp];
                --sp;
                break;
            default:
                unknownOpcode(opcode);
                break;
            }
            break;
        case 0x1000: // 0x1NNN: Jump to location NNN
            pc = opcode & 0x0FFF;
            break;
        case 0x2000: // 0x2NNN: Call subroutine at NNN
            stack[sp] = pc;
            ++sp;
            pc = opcode & 0x0FFF;
            break;
        case 0x3000: // 0x3XKK: Skip next instr. if V[X] == KK
            if((V[opcode & 0x0F00] ^ opcode & 0x00FF) == false)
                pc+=2;
            pc+=2;
            break;
        case 0x4000: // 0x4XKK: Skip next instr. if V[X] != KK
            if(V[opcode & 0x0F00] ^ opcode & 0x00FF)
                pc+=2;
            pc+=2;
            break;
        case 0x5000: // 0x5XY0: Skip next instr. if V[X] == V[Y]
            if(V[opcode & 0x0F00] ^ V[opcode & 0x00F0])
                 pc+=2;
            pc+=2;
            break;
        case 0x6000: // 0x6XKK: V[X] = KK 
            V[opcode & 0x0F00] = opcode & 0x00FF;
            pc+=2;
            break;
        case 0x7000: // 0x7XKK: V[X] += KK
            V[opcode & 0x0F00] = V[opcode & 0x0F00] + (opcode & 0x00FF);
            pc+=2;
            break;
        case 0x8000:
            switch(opcode & 0x000F) {
                case 0x0000: // 0x8XY0: V[X] = V[Y]
                    V[opcode & 0x0F00] = V[opcode & 0x00F0];
                    pc+=2;
                    break;
                case 0x0001: // 0x8XY1: V[X] OR V[Y]
                    V[opcode & 0x0F00] = V[opcode & 0x0F00] | V[opcode & 0x00F0];
                    pc+=2;
                    break;
                case 0x0002: // 0x8XY2: V[X] AND V[Y]
                    V[opcode & 0x0F00] = V[opcode & 0x0F00] & V[opcode & 0x00F0];
                    pc+=2;
                    break;
                case 0x0003: // 0x8XY3: V[X] XOR V[Y]
                    V[opcode & 0x0F00] = V[opcode & 0x0F00] ^ V[opcode & 0x00F0];
                    pc+=2;
                    break;
                case 0x0004: // 0x8XY4: V[X] ADD V[Y]
                    V[opcode & 0x0F00] = (V[opcode & 0x0F00] + V[opcode & 0x00F0]) & 0x00FF; // store only lowest 8 bits
                    if(V[opcode & 0x0F00] + V[opcode & 0x00F0] > 255)
                        V[0xF] = 1;
                    pc+=2;
                    break;
                
                case 0x0005: // 0x8XY5: V[X] SUB V[Y]
                     V[opcode & 0x0F00] = (V[opcode & 0x0F00] - V[opcode & 0x00F0]) & 0x00FF; // TODO: Store only lowest 8 bits or not?.
                    if(V[opcode & 0x0F00] > V[opcode & 0x00F0])
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    pc+=2;
                    break;
                case 0x0006: // 0x8XY6: V[X] = V[X] / 2 
                    if(V[opcode & 0x0F00] & 1) // check if last bit is 1
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    V[opcode & 0x0F00] = V[opcode & 0x0F00] >> 1;
                    pc+=2;
                    break;
                case 0x0007: // 0x8XY7: V[X] SUBN V[Y]
                     V[opcode & 0x0F00] = (V[opcode & 0x00F0] - V[opcode & 0x0F00]) & 0x00FF;
                    if(V[opcode & 0x00F0] > V[opcode & 0x0F00])
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    pc+=2;
                    break;
                case 0x000E: // 0x8XYE: V[X] = V[X] * 2 
                    if(V[opcode & 0x0F00] & 1) // check if last bit is 1
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    V[opcode & 0x0F00] = V[opcode & 0x0F00] << 1;
                    pc+=2;
                    break;
            }
            break;
        case 0x9000: // 0x9XY0: Skip next instr. if V[X] != V[Y]
            if((V[opcode & 0x0F00] ^ V[opcode & 0x00F0]) == false)
                 pc+=2;
            pc+=2;
            break;
        case 0xA000: // 0xANNN: I = NNN
            I = opcode & 0x0FFF;
            pc += 2;
            break;
        case 0xB000: // 0xBNNN: pc = NNN + V[0x0]
            pc = (opcode & 0x0FFF) + V[0x0];
        case 0xC000: // 0xCXKK: V[X] = random byte AND KK 
        // TO BO IMPLEMENTED!!!!
        case 0xD000: // 0xDXYN: Display n-byte sprite starting at memory location I at (V[X], V[Y]), V[F] = collision; 
        // TO BO IMPLEMENTED!!!!
            std::cout << "Not implemented yet!" << std::endl;
            break;
        case 0xE000:
            switch(opcode & 0x00FF){
                case 0x009E: // 0xEX9E: Skip next instr. if key with the value of V[X] is pressed
                    // TO BO IMPLEMENTED!!!!
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
            switch(opcode & 0x00FF){
                case 0x0007: // 0xFX07: V[X] = delay_timer
                    V[opcode & 0x0F00] = delay_timer;
                    pc+=2;
                    break;
                case 0x000A: // 0xFX0A: Wait for a key press, store the value of the key in V[X]
                    std::cout << "Not implemented yet!" << std::endl;
                    break;
                case 0x0015: // 0xFX15: delay_timer = V[X]
                    delay_timer = V[opcode & 0x0F00];
                    pc+=2;
                    break;
                case 0x0018: // 0xFX18: sound_timer = V[X]
                    sound_timer = V[opcode & 0x0F00];
                    pc+=2;
                    break;
                case 0x001E: // 0xFX1E: I = I + V[X]
                    I = I + V[opcode & 0x0F00];
                    pc+=2;
                    break;
                case 0x0029: // 0xFX29: I = location_of_sprite_for_digit_V[X]
                // TO BE IMPLEMENTED!!!!
                    std::cout << "Not implemented yet!" << std::endl;
                    break;
                case 0x0033: // 0xFX33: Store BCD representation of V[X] in memory locations I, I+1 and I+2
                    (*memory)[I+2] = V[opcode & 0x0F00] % 10; // ones
                    (*memory)[I+1] = (V[opcode & 0x0F00] / 10) % 10; // tens
                    (*memory)[I] = (V[opcode & 0x0F00] / 100) % 10; // hundreds
                    pc+=2;
                    break;
                case 0x0055: // 0xFX55: Store registers V[0x0] through V[X] in memory starting at location I
                    for(unsigned char i = 0x0; i <= (opcode & 0x0F00); ++i)
                        (*memory)[I] = V[i];
                    pc+=2;
                    break;
                case 0x0065: // 0xFX55: Store registers V[0x0] through V[X] in memory starting at location I
                    for(unsigned char i = 0x0; i <= (opcode & 0x0F00); ++i)
                        V[i] = (*memory)[I];
                    pc+=2;
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


