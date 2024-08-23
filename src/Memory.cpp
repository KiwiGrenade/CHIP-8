#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>

#include "Memory.hpp"
#include "utils.hpp"

Memory::Memory()
    : fileIsLoaded(false) {
    arr = std::make_shared<std::array<uint8_t, size>>();
    clear();
}

void Memory::clear() { 
    std::fill(arr->begin(), arr->end(), 0);
    fileIsLoaded = false;
}

void Memory::loadFile(const std::string& fileName) {
    clear();

    if(std::filesystem::exists(fileName) == false) {
        error("File " + fileName + " does not exist!");
    }

    prevFilename = fileName;

    auto length = std::filesystem::file_size(fileName);
    std::ifstream inputFile(fileName, std::ios_base::binary);

    if((length == 0) || (inputFile.is_open() == false) || inputFile.bad()) {
        error("Could not open file: " + fileName);
    }

    std::vector<uint8_t> buffer(length);
    inputFile.read(reinterpret_cast<char*>(buffer.data()), length);
    inputFile.close();

    for(size_t i = 0; i < buffer.size(); ++i)
        (*arr)[i+programBegin] = buffer[i];
    programSize = buffer.size();

    fileIsLoaded = true;
    inputFile.close();
}

void Memory::reloadFile() {
    loadFile(prevFilename);
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
        (*arr)[i] = fontset[i];
}

const uint16_t Memory::getOpcode(const uint16_t& pc) {
    return (*arr)[pc] << 8 | (*arr)[pc+1];
}

uint8_t& Memory::operator[](const std::size_t idx) { return (*arr)[idx]; }
const uint8_t& Memory::operator[](const std::size_t idx) const { return (*arr)[idx]; }

