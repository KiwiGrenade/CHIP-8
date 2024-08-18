#include "display.hpp"
#include <cstdint>
#include <memory>


std::array<std::array<std::shared_ptr<Pixel>, Screen::yRes_>, Screen::xRes_> Screen::pixels_;

Screen::Screen() {
    // set pixel position 
    for(uint16_t i = 0; i < yRes_; ++i) {
        float y = float(i * pixelSize_);
        for(uint16_t j = 0; j < xRes_; ++j) {
            float x = float(j * pixelSize_);
            pixels_[i][j] = std::make_unique<Pixel>(x, y);
        }
    }
}

void Screen::clear() {
    for(uint16_t i = 0; i < yRes_; ++i) {
        for(uint16_t j = 0; j < xRes_; ++j) {

        }
    }
}

std::shared_ptr<Pixel> Screen::getPixel(const uint16_t& x, const uint16_t& y) {
    return pixels_[x][y];
}


void Screen::drawSprite(
    const uint16_t& n,
    const uint16_t& x,
    const uint16_t& y,
    const uint16_t& I,
    const uint8_t V[16],
    uint8_t& VF,
    std::shared_ptr<Memory> memory) {
    VF = 0;

    for(uint16_t i = 0; i < n; ++i) {

        uint16_t yrow = (V[y] % Screen::yRes_) + i;
        uint8_t row = (*memory)[I+i];

        if(yrow >= Screen::yRes_)
            continue;

        for(uint8_t j = 0; j < 8; ++j) {

            uint16_t xcol = (V[x] % Screen::xRes_) + j;
            bool currbit = (row >> (8-j-1)) & 1;

            if(xcol >= Screen::xRes_)
                continue;

            std::shared_ptr<Pixel> pixel = getPixel(xcol, yrow);
            if(pixel->state_ == true && currbit)
                VF = 1;
            pixel->state_ = pixel->state_ ^ currbit;
        }
    }
}

