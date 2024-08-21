#include "display.hpp"
#include <cstdint>
#include <memory>

QReadWriteLock Screen::pixelsLock_;
std::shared_ptr<std::array<std::array<bool, Screen::yRes_>, Screen::xRes_>> Screen::pixels_;

Screen::Screen() {
    // set pixel position 
    pixelsLock_.lockForWrite();
    pixels_ = std::make_shared<std::array<std::array<bool, yRes_>, xRes_>>();
    pixelsLock_.unlock();
}

void Screen::clear() {
    pixelsLock_.lockForWrite();
    for(uint16_t i = 0; i < yRes_; ++i) {
        for(uint16_t j = 0; j < xRes_; ++j) {
            (*pixels_)[j][i] = false;
        }
    }
    pixelsLock_.unlock();
}

bool Screen::getPixel(const uint16_t& x, const uint16_t& y) {
    pixelsLock_.lockForRead();
    bool pixel = (*pixels_)[x][y];
    pixelsLock_.unlock();
    return pixel;
}

void Screen::setPixel(const uint16_t& x, const uint16_t& y, const bool state) {
    pixelsLock_.lockForWrite();
    (*pixels_)[x][y] = state;
    pixelsLock_.unlock();
}


