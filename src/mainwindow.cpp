#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <memory>
#include <iostream>
#include <QImage>
#include <QFileDialog>
#include "chip8.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = std::make_shared<QGraphicsScene>(this);
    /*scene->addPixmap(QPixmap::fromImage(*Screen::image_));*/
    ui->graphicsView->setScene(scene.get());
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::onClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionLoad_triggered() {
    chip8::clear();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose ROM"), "", tr(" ROMs (*.ch8)"));
    if(fileName.isEmpty()) {
        std::cerr << "Could not open file: " << fileName.toStdString() << std::endl;
        exit(2);
        /*error("Could not open file: " + fileName.toStdString());*/
    }
    chip8::loadFile(fileName.toStdString());
    chip8::getMemory().printProgram();
    chip8::start();
}

void MainWindow::on_actionReload_triggered() {

}

void MainWindow::onClicked() {
    chip8::emulateCycle();
    if(chip8::getDrawFlag())
        scene->addPixmap(QPixmap::fromImage(*Screen::image_));
}

