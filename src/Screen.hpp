#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <cstdint>
#include <memory>

#include <QReadWriteLock>

struct Screen {
    Screen();
    ~Screen() = default;

    void clear();
    static bool getPixel(const uint16_t& x, const uint16_t& y);
    static void setPixel(const uint16_t& x, const uint16_t& y, bool state);

    static constexpr uint16_t   xRes_ = 64;
    static constexpr uint16_t   yRes_ = 32;
    static constexpr uint16_t   pixelSize_ = 10;

    static QReadWriteLock pixelsLock_;
    static  std::shared_ptr<
                std::array<
                    std::array<bool, Screen::yRes_>,
                Screen::xRes_>>
            pixels_;
};

#endif // DISPLAY_HPP

