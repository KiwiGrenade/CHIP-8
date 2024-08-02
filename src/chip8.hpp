#ifndef CHIP8_HPP
#define CHIP8_HPP

#include <memory>
#include <SFML/Graphics/RenderWindow.hpp>
#include "display.hpp"
#include "memory.hpp"

struct Chip8 {
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
    static bool     pause_;

    std::unique_ptr<Memory> memory_;
    std::unique_ptr<Screen> screen_;

    Chip8(); // initialize all components
    void loadFile(const std::string& filename);
    void emulateCycle();
    void drawScreen(sf::RenderWindow& window);
private:
    sf::Keyboard::Key getKey(const unsigned char& x); 
};

#endif //CHIP8_HPP
