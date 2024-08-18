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

    myChip8 = std::make_unique<Chip8>();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionLoad_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose ROM"), "", tr(" ROMs (*.ch8)"));
    if(fileName.isEmpty()) {
        std::cerr << "Could not open file: " << fileName.toStdString() << std::endl;
        exit(2);
        /*error("Could not open file: " + fileName.toStdString());*/
    }
    myChip8->loadFile(fileName.toStdString());
    myChip8->getMemory().printProgram();

    /*for(int i = 0; i < 256; ++i) {*/
        /*chip8::emulateCycle();*/
	    /*deltaTime = clock.restart();*/
	    /**/
	    /*if(deltaTime > sf::milliseconds(100))*/
	    /*    deltaTime = sf::milliseconds(100);*/
	    /**/
	    /*accuTime += deltaTime;*/
	    /*const sf::Time one_sixtieth_of_a_second = sf::microseconds(16670);*/
	    /**/
	    /*// Goal: 500Hz clock speed (500 cycle emulations per second) with 60Hz updates*/
	    /*// for every 1/60s -> update timers*/
	    /*for(; accuTime >= one_sixtieth_of_a_second; accuTime -= one_sixtieth_of_a_second) {*/
	    /**/
	    /*    myChip8->updateTimers();*/
	    /**/
	    /*    //  60*8 =(approx) 500*/
	    /*    for(size_t i = 0; i < 8 && (!myChip8->getIsWaitingForKeyboardInput()); i++) {*/
	    /*        myChip8->emulateCycle(event);*/
	    /**/
	    /*        if(myChip8->getDrawFlag())*/
	    /*            myChip8->drawScreen(window);*/
        /*if(chip8::getDrawFlag())*/
            /*scene->addPixmap(QPixmap::fromImage(*Screen::image_));*/
    /*}*/
}

void MainWindow::on_actionReload_triggered() {

}

void MainWindow::on_actionStartEmulator_triggered() {
    if(myChip8->getIsRunnig())
        return;
    myChip8->start();
}

void MainWindow::on_actionStopEmulator_triggered() {
    if(!myChip8->getIsRunnig())
        return;
    myChip8->stop();
}

void MainWindow::on_actionStepEmulator_triggered() {
    myChip8->emulateCycle();
}

void MainWindow::on_actionPauseEmulator_triggered() {
    std::cout << "PAUSE" << std::endl;
}
