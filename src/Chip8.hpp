#ifndef CHIP8_HPP
#define CHIP8_HPP

#include <cstdint>
#include <set>

#include <QThread>
#include <QReadWriteLock>

#include "Screen.hpp"
#include "Memory.hpp"

class Chip8 : public QThread {
    Q_OBJECT
public:
    Chip8();
    ~Chip8() = default;

    inline Memory& getMemory() { return *memory; }
    inline Screen& getScreen() { return *screen; }
    static inline bool getDrawFlag() { return drawFlag; }
    inline bool getIsWaitingForKeyboardInput() { return isWaitingForKeyboardInput; }
    inline uint8_t getDelayTimer() { return delayTimer; }
    inline uint8_t getSoundTimer() { return soundTimer; }
    inline bool isPaused() { return paused; }
    inline bool isAlive() { return alive; }
    static void updateTimers();
    void addKeyDown(const unsigned char& keyVal);
    void removeKeyDown(const unsigned char& keyVal);
    
    void loadFile(const QByteArray& fileContent);
    void emulateCycle();
    void clear();
    
    // emulator control 
    void restart();
    void pause();
    void unPause();
    void stop();
    void run() override; // start
 
private: 
    size_t      nCycle;
    uint16_t    lastX;
    uint16_t    pc;      // program counter
    uint16_t    opcode;  // current opcode (opcodes are 2 bytes)
    uint16_t    I;       // memory pointer
    uint16_t    sp;      // stack pointer
    uint8_t     V[16];    // 16 * 1 byte registers (VF is carry flag)
    uint16_t    stack[16];
    uint8_t     key[16];
    static bool drawFlag;

    static uint8_t         soundTimer;
    static QReadWriteLock  soundTimerLock;
    static uint8_t         delayTimer;
    static QReadWriteLock  delayTimerLock;

    static bool        isWaitingForKeyboardInput;
    static bool        paused;
    static bool        alive;

    std::set<char> keysDown;
    QReadWriteLock keysDownLock;

    std::string ROMFileName;

    std::shared_ptr<Memory> memory;
    std::shared_ptr<Screen> screen;

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
};

#endif //CHIP8_HPP

