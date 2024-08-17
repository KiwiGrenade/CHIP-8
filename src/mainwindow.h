#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <QMainWindow>
#include <QGraphicsScene>

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
    void on_actionLoad_triggered();
    void on_actionReload_triggered();
    void onClicked();

private:
    Ui::MainWindow *ui;
    std::shared_ptr<QGraphicsScene> scene;
};
#endif // MAINWINDOW_H

