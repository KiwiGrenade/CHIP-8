#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <QMainWindow>
#include <QKeyEvent>

#include "Chip8.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // This slot naming convention automatically connects actions to slots
    void on_actionLoad_triggered();
    void on_actionReload_triggered();
    void on_actionStartEmulator_triggered();
    void on_actionStopEmulator_triggered();
    void on_actionStepEmulator_triggered();
    void on_actionPauseEmulator_triggered();

private:
    void keyReleaseEvent(QKeyEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void closeEvent(QCloseEvent* event);

    Ui::MainWindow *ui;
    std::unique_ptr<Chip8> myChip8;
    std::map<char, unsigned char> keyMap {
        {'x', 0x0},
        {'1', 0x1},
        {'2', 0x2},
        {'3', 0x3},
        {'q', 0x4},
        {'w', 0x5},
        {'e', 0x6},
        {'a', 0x7},
        {'s', 0x8},
        {'d', 0x9},
        {'z', 0xA},
        {'c', 0xB},
        {'4', 0xC},
        {'r', 0xD},
        {'f', 0xE},
        {'v', 0xF}
    };
};
#endif // MAINWINDOW_H

