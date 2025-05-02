#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    sj = new send_json(this);
    ui->page->addWidget(sj);
    ui->page->setCurrentWidget(sj);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_MainWidget_Btn_clicked()
{
    qDebug() << "当前堆栈容器中的页面数量：" << ui->page->count();
    qDebug() << "当前页面索引：" << ui->page->currentIndex();
    ui->page->setCurrentWidget(sj);
}

