#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include <memory>
#include <iostream>
#include <QImage>
#include <QFileDialog>
#include <qtpreprocessorsupport.h>
#include "Chip8.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    myChip8 = std::make_unique<Chip8>();
    keyMap = std::make_unique<std::map<char, unsigned char>>();

    keyMap->insert({'x', 0x0});
    keyMap->insert({'1', 0x1});
    keyMap->insert({'2', 0x2});
    keyMap->insert({'3', 0x3});
    keyMap->insert({'q', 0x4});
    keyMap->insert({'w', 0x5});
    keyMap->insert({'e', 0x6});
    keyMap->insert({'a', 0x7});
    keyMap->insert({'s', 0x8});
    keyMap->insert({'d', 0x9});
    keyMap->insert({'z', 0xA});
    keyMap->insert({'c', 0xB});
    keyMap->insert({'4', 0xC});
    keyMap->insert({'r', 0xD});
    keyMap->insert({'f', 0xE});
    keyMap->insert({'v', 0xF});
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
        if(keyMap->contains(key)) {
            myChip8->addKeyDown((*keyMap)[key]);
        }
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent* event) {
    if(event->text().size() >= 1)
    {
        char key = event->text().at(0).toLatin1();
        if(keyMap->contains(key)) {
            myChip8->removeKeyDown((*keyMap)[key]);
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

