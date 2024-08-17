#ifndef CHIP8_HPP
#define CHIP8_HPP

#include <cstdint>
#include "display.hpp"
#include "memory.hpp"

namespace chip8 {

/*void drawScreen(MainWindow& window);*/
void loadFile(const std::string& filename);
void emulateCycle();
/*void loadKeyToV(const sf::Event& event);*/
void clear();
void updateTimers();
void setUp(const std::string& filename);
void start();
void stop();

Memory& getMemory(); 
Screen& getScreen();
bool getDrawFlag();
bool getIsWaitingForKeyboardInput();
uint8_t getDelayTimer();
uint8_t getSoundTimer();

// pseudo private
namespace {
void unknownOpcode(const uint16_t& opcode);
void drawSprite(
    const uint16_t n,
    const uint16_t x,
    const uint16_t y,
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
} // private namespace
} // chip8

#endif //CHIP8_HPP

