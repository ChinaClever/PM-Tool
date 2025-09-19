#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
    connect(navigation, &navarwid::navBarSig, this, &MainWindow::navBarSlot);
}

void MainWindow::init()
{

    navigation = new navarwid(ui->barWid);
    mainpage = new mainWid(this);
    ui->stackedWidget->addWidget(mainpage);

    from = new Form(this);
    ui->stackedWidget->addWidget(from);
    frrom = new Forrm(this);
    ui->stackedWidget->addWidget(frrom);
    from1 = new Form1(this);
    ui->stackedWidget->addWidget(from1);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::navBarSlot(int id)
{
    qDebug()<<"current page:  "<<id;
    ui->stackedWidget->setCurrentIndex(id);
}
