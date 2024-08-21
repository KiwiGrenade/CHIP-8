#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <array>
#include <memory>
#include <vector>
#include <cstdint>
#include <string>

/* CHIP-8 has 4KB memory (4096 bytes), from location 0x000 (0) to 0xFFF (4095):
     * + 0x000 (0) to 0x1FF (511) - CHIP-8 interpreter
     * + 0x200 (512) to 0xFFF (4095) - program memory (ETI 660 programs start at 0x600 (1536))*/

struct Memory {
    Memory();
    ~Memory() = default;

    void clear();
    void printProgram();
    void loadFontset();
    void loadFile(const std::string& filename);
    void reloadFile();
    inline bool isFileLoaded() { return fileIsLoaded; }

    const uint16_t getOpcode(const uint16_t& pc);

    uint8_t& operator[](const std::size_t idx);
    const uint8_t& operator[](const std::size_t idx) const;

    bool fileIsLoaded;
    static constexpr uint16_t size = 4096;
    static constexpr uint16_t fontsetSize = 80;
    static constexpr uint16_t programBegin = 512;
    uint16_t programSize = 0;
    std::string prevFilename;

    static constexpr uint8_t fontset [fontsetSize] { 
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    
    std::shared_ptr<std::array<uint8_t, size>> arr;
};



#endif // MEMORY_HPP
