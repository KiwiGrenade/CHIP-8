#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <memory>
#include <iostream>
#include <QImage>
#include <QFileDialog>
#include <qmainwindow.h>
#include <qwidget.h>
#include "chip8.hpp"

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
    myChip8->setRunning();
    myChip8->start();
}

void MainWindow::on_actionStopEmulator_triggered() {
    myChip8->stop();
}

void MainWindow::on_actionStepEmulator_triggered() {
    myChip8->emulateCycle();
}

void MainWindow::on_actionPauseEmulator_triggered() {
    myChip8->stop();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if(myChip8->getIsRunnig())
    {
        myChip8->stop();
        myChip8->wait();
    }
    QMainWindow::closeEvent(event);
}

