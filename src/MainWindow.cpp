#include "MainWindow.hpp"
#include "ui_MainWindow.h"

#include <memory>

#include <QImage>
#include <QFileDialog>

#include "Chip8.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    myChip8 = std::make_unique<Chip8>();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionLoad_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose ROM"), "", tr(" ROMs (*.ch8)"));

    if(!fileName.isEmpty()) {
        myChip8->loadFile(fileName.toStdString());
        myChip8->getMemory().printProgram();
    }
}

void MainWindow::on_actionReload_triggered() {

}

void MainWindow::on_actionStartEmulator_triggered() {
    myChip8->start();
}

void MainWindow::on_actionStopEmulator_triggered() {
    myChip8->stop();
}

void MainWindow::on_actionStepEmulator_triggered() {
    myChip8->emulateCycle();
}

void MainWindow::on_actionPauseEmulator_triggered() {
    if(myChip8->isPaused())
        myChip8->unPause();
    else
        myChip8->pause();
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    if(event->text().size() >= 1)
    {
        char key = event->text().at(0).toLatin1();
        if(keyMap.contains(key)) {
            myChip8->addKeyDown(keyMap[key]);
        }
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent* event) {
    if(event->text().size() >= 1)
    {
        char key = event->text().at(0).toLatin1();
        if(keyMap.contains(key)) {
            myChip8->removeKeyDown(keyMap[key]);
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    Q_UNUSED(event)

    if(myChip8->isAlive() && !myChip8->isPaused())
    {
        myChip8->stop();
        myChip8->wait();
    }
}

