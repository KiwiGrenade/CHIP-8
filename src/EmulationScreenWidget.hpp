#ifndef EMULATION_SCREEN_WIDGET_HPP
#define EMULATION_SCREEN_WIDGET_HPP

#include <QWidget>
#include <QTimer>

#include "Screen.hpp"

class EmulationScreenWidget : public QWidget, public Screen {
    Q_OBJECT
public:
    EmulationScreenWidget(QWidget *parent = nullptr);

private slots:
    void forceRepaint();

protected:
    void paintEvent(QPaintEvent* event);

    QTimer repaintTimer;

private:
    constexpr static int timerInterval_ms = 17;
};

#endif // !EMULATION_SCREEN_WIDGET_HPP
