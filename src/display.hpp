#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <algorithm>
#include <cstdint>
#include <memory>
#include <array>
#include <QImage>
#include <QColor>
#include "memory.hpp"
#include "component.hpp"

struct Pixel {
    Pixel(const uint16_t& x, const uint16_t& y) : 
        x_(x),
        y_(y),
        state_(false) {};

    uint16_t    x_;
    uint16_t    y_;
    bool        state_;
};



struct Screen : Component {
    Screen();
    ~Screen() = default;

    void clear();

    static constexpr uint16_t   xRes_ = 64;
    static constexpr uint16_t   yRes_ = 32;
    static constexpr uint16_t   pixelSize_ = 10;
    /*static constexpr uint16_t   width_ = resX_ * pixelSize_;*/
    /*static constexpr uint16_t   height_ = resY_ * pixelSize_;*/
    /*static constexpr uint32_t   nPixels_ = width_ * height_;*/
    static std::shared_ptr<Pixel> getPixel(const uint16_t& x, const uint16_t& y);

    static void drawSprite(
        const uint16_t& n,
        const uint16_t& x,
        const uint16_t& y,
        const uint16_t& I,
        const uint8_t V[16],
        uint8_t& VF,
        std::shared_ptr<Memory> memory);

    static std::array<std::array<std::shared_ptr<Pixel>, yRes_>, xRes_> pixels_;
};

#endif // DISPLAY_HPP

