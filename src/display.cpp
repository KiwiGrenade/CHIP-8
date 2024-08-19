#include "display.hpp"
#include <cstdint>
#include <memory>

QReadWriteLock Screen::pixelsLock_;
std::shared_ptr<std::array<std::array<bool, Screen::xRes_>, Screen::yRes_>>Screen::pixels_;

Screen::Screen() {
    // set pixel position 
    pixels_ = std::make_shared<std::array<std::array<bool, xRes_>, yRes_>>(); 
}

void Screen::clear() {
    for(uint16_t i = 0; i < yRes_; ++i) {
        for(uint16_t j = 0; j < xRes_; ++j) {
            (*pixels_)[i][j] = false;
        }
    }
}

bool& Screen::getPixel(const uint16_t& x, const uint16_t& y) {
    return (*pixels_)[y][x];
}


