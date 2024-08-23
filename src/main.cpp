#include <memory>
#include <QApplication>

#include "MainWindow.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    auto w = std::make_unique<MainWindow>();
    w->show();
    return a.exec();
}

