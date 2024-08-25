#include <algorithm>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>
#include <vector>

#include "Memory.hpp"
#include "utils.hpp"

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

void Memory::loadFile(const std::string& fileName) {
    clear();

    if(!std::filesystem::exists(fileName)) {
        error("File " + fileName + " does not exist!");
    }

    prevFilename = fileName;

    std::ifstream inputFile(fileName, std::ios_base::binary);

    if(!inputFile.good()) {
        error("Could not open file: " + fileName);
    }

    // read file to a buffer
    auto length = std::filesystem::file_size(fileName);
    auto buffer = std::make_unique<std::vector<uint8_t>>(length);
    inputFile.read(reinterpret_cast<char*>(buffer->data()), length);
    inputFile.close();

    std::copy(buffer->begin(), buffer->end(), arr->begin() + programBegin);

    /*programSize = buffer->size();*/
    programSize = length;
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

