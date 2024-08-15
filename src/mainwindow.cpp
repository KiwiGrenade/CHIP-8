#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <memory>
#include <QImage>


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

