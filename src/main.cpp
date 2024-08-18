#include <memory>
/*#include <string>*/
#include <QApplication>
#include <QWidget>
#include <QGraphicsView>
#include "utils.hpp"
#include "mainwindow.h"
#include "chip8.hpp"

bool Options::verbose = false;
bool Options::debug = false;

int main(int argc, char *argv[])
{
    Options::verbose = true;
    QApplication a(argc, argv);
    auto w = std::make_unique<MainWindow>();
    w->show();
    return a.exec();
}

