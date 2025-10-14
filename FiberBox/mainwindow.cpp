#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "databaseinitializer.h"
#include <QMessageBox>
#include <QDebug>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initDb();
    init();
    connect(navigation, &navarwid::navBarSig, this, &MainWindow::navBarSlot);
}

void MainWindow::initDb()
{
    DatabaseInitializer init;
    if (!init.initialize()) {
        qDebug() << "[DB] init failed";
        // ：QMessageBox::critical(this, "错误", "数据库初始化失败！");
        return;
    }
    qDebug() << "[DB] init ok (db + templates table ready)";
}

void MainWindow::init()
{
    navigation = new navarwid(ui->barWid);
    mainpage = new mainWid(this);
    ui->stackedWidget->addWidget(mainpage);
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
