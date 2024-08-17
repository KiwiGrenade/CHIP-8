#include "display.hpp"
#include <memory>

std::shared_ptr<QImage> Screen::image_;

Pixel::Pixel(const uint16_t& x, const uint16_t& y) :
    x_(x),
    y_(y),
    state_(false)
{}

void Pixel::setState(const bool& state) {
    state_ = state;
    QColor color =  state ? ON : OFF;
    for(size_t i = 0; i < Screen::pixelSize_; i++) {
        for(size_t j = 0; j < Screen::pixelSize_; j++) {
            Screen::image_->setPixelColor(x_ + j, y_ + i, color);
        }
    }
}

Screen::Screen() {
    image_ = std::make_shared<QImage>(width_ * pixelSize_, height_ * pixelSize_, QImage::Format_RGB32);
    image_->fill(Pixel::OFF);

    // set pixel position 
    for(uint16_t i = 0; i < height_; ++i) {
        float y = float(i * pixelSize_);
        for(uint16_t j = 0; j < width_; ++j) {
            float x = float(j * pixelSize_);
            pixels_[i * width_ + j] = std::make_shared<Pixel>(x, y);
        }
    }
}

void Screen::clear() {
    for(auto a : pixels_)
        a->setState(false);
}

std::shared_ptr<Pixel> Screen::getPixel(const uint16_t& x, const uint16_t& y) {
    return pixels_[(y % height_) * width_ + (x % width_)];
}

