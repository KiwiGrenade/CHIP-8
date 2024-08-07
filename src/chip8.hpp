#ifndef CHIP8_HPP
#define CHIP8_HPP

#include <memory>
#include <SFML/Graphics/RenderWindow.hpp>
#include "SFML/Window/Keyboard.hpp"
#include "display.hpp"
#include "memory.hpp"

class Chip8 {

    size_t nCycle_;
    
    bool            drawFlag_;
    unsigned short  pc_;      // program counter
    unsigned short  opcode_;  // current opcode (opcodes are 2 bytes)
    unsigned short  I_;       // memory pointer
    unsigned short  sp_;      // stack pointer
    unsigned char   V_[16];    // 16 * 1 byte registers (VF is carry flag)
    unsigned char   sound_timer_;
    unsigned char   delay_timer_;
    unsigned short  stack_[16];
    unsigned char   key_[16];
    bool            waitingForKeyboardInput_;

    std::unique_ptr<Memory> memory_;
    std::unique_ptr<Screen> screen_;

    const sf::Keyboard::Key charToKey(const unsigned char& x);
    const unsigned char getKeyToChar(const sf::Keyboard::Scancode& key);
    void drawSprite(
        const unsigned short n,
        const unsigned short x,
        const unsigned short y,
        unsigned char& VF);

public:

    Chip8(); // initialize all components

    void drawScreen(sf::RenderWindow& window);
    void loadFile(const std::string& filename);
    void emulateCycle(const sf::Event& event);

    inline Memory& getMemory() {
        return *memory_;
    }

    inline Screen& getScreen() {
        return *screen_;
    }

    inline bool getDrawFlag() {
        return drawFlag_;
    }

    inline bool isWaitingForKeyboardInput() {
        return waitingForKeyboardInput_;
    }

};

#endif //CHIP8_HPP
