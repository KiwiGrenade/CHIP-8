#include "memory.hpp"
#include <iostream>

void Memory::clear() { std::fill(arr.begin(), arr.end(), 0); }

void Memory::loadProgram(std::vector<unsigned char>& program) {
    for(size_t i = 0; i < program.size(); ++i)
        arr[i+programBegin] = program[i];
    programSize = program.size();
}

void Memory::printProgram() {
    std::cout   << "###### Program START ###### " << std::endl\
                << "opcode index : [memory addresses] : <opcode>" << std::endl;
    for(unsigned short i = programBegin; i < programBegin+programSize; i+=2)
        std::cout << std::dec << i - programBegin  << " : [" << i << "-" << i+1 << "] : " << std::hex << getOpcode(i) << std::endl;
    std::cout << "###### Program END ###### " << std::endl;
}

void Memory::loadFontset() {
    for(unsigned short i ; i < 80; ++i)
        arr[i] = fontset[i];
}

const unsigned short Memory::getOpcode(const unsigned short& pc) {
    return arr[pc] << 8 | arr[pc+1];
}

unsigned char& Memory::operator[](const std::size_t idx) { return arr[idx]; }
const unsigned char& Memory::operator[](const std::size_t idx) const { return arr[idx]; }


