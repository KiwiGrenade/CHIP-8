#include <algorithm>
#include <cstdint>
#include <iostream>
#include <memory>
#include <qbytearrayalgorithms.h>
#include <qstringview.h>

#include "Memory.hpp"

uint16_t Memory::programSize;

Memory::Memory()
    : fileIsLoaded(false) {
    programSize = 0;
    arr = std::make_shared<std::array<uint8_t, memorySize>>();
    clear();
    std::copy(fontset.begin(), fontset.end(), arr->begin());
}

void Memory::clear() {
    std::fill(arr->begin() + fontsetSize, arr->end(), 0);
    fileIsLoaded = false;
}

void Memory::loadFile(const QByteArray& fileContent) {
    clear();

    programSize = fileContent.size() - 1;

    for(uint16_t i = 0; i < programSize; ++i) {
        arr->at(i + programBegin) = fileContent.at(i);
    }

    fileIsLoaded = true;
}

void Memory::printProgram() {
    std::cout   << "###### Program START ###### " << std::endl
                << "opcode index : [memory addresses] : <opcode>" << std::endl;
    
    for(uint16_t i = programBegin; i < programBegin+programSize; i+=2)
        std::cout << std::dec << i - programBegin  << " : [" << i << "-" << i+1 << "] : " << std::hex << getOpcode(i) << std::endl;
    
    std::cout << "###### Program END ###### " << std::endl;
}

const uint16_t Memory::getOpcode(const uint16_t& pc) {
    return (*arr)[pc] << 8 | (*arr)[pc+1];
}

