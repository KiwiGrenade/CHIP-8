#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <QMainWindow>
#include <QGraphicsScene>
#include "chip8.hpp"

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
    Ui::MainWindow *ui;
    std::shared_ptr<QGraphicsScene> scene;
    std::unique_ptr<Chip8> myChip8;
};
#endif // MAINWINDOW_H

