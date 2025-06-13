#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    intiWid();

}
void MainWindow::intiWid()
{
    mIPNavarWid = new IP_NavarWid(ui->barWid);

    mIPJsonData = new IP_JsonData();
    mMainPage = new mp_mainPage();
    mProData = new mp_data();
    mBusData = new busUiData();

    ui->stackedWid->addWidget(mMainPage);
    ui->stackedWid->addWidget(mIPJsonData);
    ui->stackedWid->addWidget(mProData);
    ui->stackedWid->addWidget(mBusData);

    ui->stackedWid->setCurrentIndex(3);
    connect(mIPNavarWid, &IP_NavarWid::navBarSig, this, &MainWindow::navBarSlot);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::navBarSlot(int id)
{
    qDebug()<<"current page:  "<<id;
    ui->stackedWid->setCurrentIndex(id);
}
