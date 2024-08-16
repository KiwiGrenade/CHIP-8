#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <memory>
#include <iostream>
#include <QImage>
#include <QFileDialog>
/*#include "utils.hpp"*/

MainWindow::MainWindow(std::shared_ptr<QImage> image, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    /**/
    /*int sizeX = 640;*/
    /*int sizeY = 320;*/
    /**/
    /*QImage image = QImage(sizeX, sizeY, QImage::Format_Mono);*/
    /**/
    /*image.fill(0);*/
    scene = std::make_shared<QGraphicsScene>(this);

    scene->addPixmap(QPixmap::fromImage(*image));

    /*graphics->addPixmap(QPixmap::fromImage(image));*/

    ui->graphicsView->setScene(scene.get());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionLoad_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open CHIP-8 ROM"), "", tr(" ROMs (*.ch8)"));
    if(fileName.isEmpty()) {
        //TODO: Change this to error() from utils.hpp. Problem with including utils.hpp
        std::cerr << "Could not open file: " << fileName.toStdString() << std::endl;
        /*error("Could not open file: " + fileName.toStdString());*/
    }
}

void MainWindow::on_actionReload_triggered() {

}
