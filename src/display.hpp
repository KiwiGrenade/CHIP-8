#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <cstdint>
#include <memory>
#include <vector>
#include <QReadWriteLock>
#include "memory.hpp"
#include "component.hpp"

struct Screen : Component {
    Screen();
    ~Screen() = default;

    void clear();
    static bool& getPixel(const uint16_t& x, const uint16_t& y);

    static constexpr uint16_t   xRes_ = 64;
    static constexpr uint16_t   yRes_ = 32;
    static constexpr uint16_t   pixelSize_ = 10;

    static QReadWriteLock pixelsLock_;
    static std::shared_ptr<std::array<std::array<bool, xRes_>, yRes_>> pixels_;
    /*static std::array<std::array<std::shared_ptr<Pixel>, yRes_>, xRes_> pixels_;*/
};

#endif // DISPLAY_HPP

