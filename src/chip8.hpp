#ifndef CHIP8_HPP
#define CHIP8_HPP

#include <cstdint>
#include <filesystem>
#include "display.hpp"
#include "memory.hpp"
#include <QThread>

class Chip8 : public QThread {
public:
    Chip8();
    ~Chip8() = default;
    inline Memory& getMemory() { return *memory; }
    inline Screen& getScreen() { return *screen; }
    inline bool getDrawFlag() { return drawFlag; }
    inline bool getIsWaitingForKeyboardInput() { return isWaitingForKeyboardInput; }
    inline uint8_t getDelayTimer() { return delaytimer; }
    inline uint8_t getSoundTimer() { return soundtimer; }
    inline void start() { isRunning = true; }
    inline void stop() { if(!isRunning){ return; } isRunning = false; }
    inline bool getIsRunnig() { return isRunning; }
    void loadFile(const std::string& filename);
    void emulateCycle();
    void clear();
 
private: 
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

    std::shared_ptr<Memory> memory;
    std::unique_ptr<Screen> screen;

        /*void drawScreen(MainWindow& window);*/
        /*void loadKeyToV(const sf::Event& event);*/
    void updateTimers();
    void unknownOpcode(const uint16_t& opcode);
    void drawSprite(
        const uint16_t& n,
        const uint16_t& x,
        const uint16_t& y,
        uint8_t& VF);
    void printData( 
        const uint16_t& x,
        const uint16_t& y,
        const uint16_t& n,
        const uint16_t& kk,
        const uint16_t& VF,
        const uint16_t& nnn);

    /*const sf::Keyboard::Key charToKey(const uint8_t& x);*/
    /*const uint8_t getKeyToChar(const sf::Keyboard::Scancode& key);*/
};

#endif //CHIP8_HPP

