#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <cstdint>
#include <memory>
#include <array>
#include <QImage>
#include <qcolor.h>
/*#include "SFML/System/Vector2.hpp"*/
/*#include <SFML/Graphics.hpp>*/
/*#include <SFML/Graphics/RectangleShape.hpp>*/
#include "component.hpp"

class Pixel {
public:
    Pixel(const uint16_t& x, const uint16_t& y);

    inline const bool getState() { return state_; };
    void setState(const bool& state);

    ~Pixel() = default;

    static constexpr QColor ON = QColorConstants::White;
    static constexpr QColor OFF = QColorConstants::Black;

private:
    uint16_t    x_;
    uint16_t    y_;
    bool        state_;
};

struct Screen : Component {
    static constexpr uint16_t   height_ = 32;
    static constexpr uint16_t   width_ = 64;
    static constexpr uint16_t   nPixels_ = width_ * height_;
    static constexpr uint16_t   pixelSize_ = 10;

    std::array<std::shared_ptr<Pixel>, nPixels_> pixels_;
    static std::shared_ptr<QImage> image_;

    Screen();

    void clear();
    std::shared_ptr<Pixel> getPixel(const uint16_t& x, const uint16_t& y);

    ~Screen() = default;
};

#endif // DISPLAY_HPP

