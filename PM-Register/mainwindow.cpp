#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>
#include <QDir>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mkeygen(new Keygen(this))

{
    ui->setupUi(this);
   // this->setFixedSize(1260, 860);

    initWid();
    connect(mNavar, &navarwid::navBarSig, this, &MainWindow::navBarSlot);
    loadAllSettings(); //读写配置文件
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::initWid()
{
    mNavar = new navarwid(ui->barWid);
    ui->stackedWidget->addWidget(mkeygen);
    mSetupWid = new Setup_MainWid(this);
    ui->stackedWidget->addWidget(mSetupWid);
    mLog = new LogMainWid(this);
    ui->stackedWidget->addWidget(mLog);

}

void MainWindow::navBarSlot(int id)
{
    qDebug()<<"current page:  "<<id;
    ui->stackedWidget->setCurrentIndex(id);
}

void MainWindow::saveAllSettings() {
    QString configPath = QCoreApplication::applicationDirPath() + "/config.ini";
//    qDebug() << "Saving to:" << configPath;

    QSettings settings(configPath, QSettings::IniFormat); // 这里使用完整路径

    settings.sync(); // 同步写磁盘
}

void MainWindow::loadAllSettings() {

    QSettings settings(CfgCom::bulid()->pathOfData("cfg.ini"), QSettings::IniFormat);

    mkeygen->loadSettings(settings);;

}

void MainWindow::closeEvent(QCloseEvent *event) {
    saveAllSettings();
    QMainWindow::closeEvent(event);
}
