#include "EmulationScreenWidget.hpp"
#include <cstdint>
#include <QPainter>
#include <QWidget>

EmulationScreenWidget::EmulationScreenWidget(QWidget *parent) :
    QWidget(parent) {
    repaintTimer.setInterval(timerInterval_ms);
    connect(&repaintTimer, SIGNAL(timeout()), this, SLOT(forceRepaint()));
    repaintTimer.start();
}

// Draw pixels on the screen 
void EmulationScreenWidget::paintEvent(QPaintEvent * event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setPen(Qt::blue);

    const uint16_t& xRes = Screen::xRes_;
    const uint16_t& yRes = Screen::yRes_;

    uint32_t pixelWidth = width() / xRes;
    uint32_t pixelHeight = height() / yRes;

    uint32_t xOffset = width() % xRes / 2;
    uint32_t yOffset = height() % yRes / 2;
    
    for(uint16_t y = 0; y < yRes; ++y) {
        for(uint16_t x = 0; x < xRes; ++x) {
            if(!Screen::getPixel(x, y))
                // TODO: Render pixel as a class (derived from rectangle)?
                painter.fillRect(pixelWidth * x + xOffset, pixelHeight * y + yOffset, pixelWidth, pixelHeight, Qt::black);
        }
    }
}

void EmulationScreenWidget::forceRepaint() {
    repaint();
}
