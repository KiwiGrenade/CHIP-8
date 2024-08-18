#include <iostream>
#include "memory.hpp"

void Memory::clear() { std::fill(arr.begin(), arr.end(), 0); }

void Memory::loadProgram(std::vector<uint8_t>& program) {
    for(size_t i = 0; i < program.size(); ++i)
        arr[i+programBegin] = program[i];
    programSize = program.size();
}

void Memory::printProgram() {
    std::cout   << "###### Program START ###### " << std::endl
                << "opcode index : [memory addresses] : <opcode>" << std::endl;
    
    for(uint16_t i = programBegin; i < programBegin+programSize; i+=2)
        std::cout << std::dec << i - programBegin  << " : [" << i << "-" << i+1 << "] : " << std::hex << getOpcode(i) << std::endl;
    
    std::cout << "###### Program END ###### " << std::endl;
}

void Memory::loadFontset() {
    for(uint16_t i = 0; i < fontsetSize; ++i)
        arr[i] = fontset[i];
}

const uint16_t Memory::getOpcode(const uint16_t& pc) {
    return arr[pc] << 8 | arr[pc+1];
}

uint8_t& Memory::operator[](const std::size_t idx) { return arr[idx]; }
const uint8_t& Memory::operator[](const std::size_t idx) const { return arr[idx]; }

