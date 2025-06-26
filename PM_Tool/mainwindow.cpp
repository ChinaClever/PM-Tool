#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mBusbulk(new busBulk(this))
{
    ui->setupUi(this);
    intiWid();
    loadAllSettings(); //读写配置文件

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

    ui->stackedWid->setCurrentIndex(0);
    connect(mIPNavarWid, &IP_NavarWid::navBarSig, this, &MainWindow::navBarSlot);
}

void MainWindow::saveAllSettings() {

    QString configPath = QCoreApplication::applicationDirPath() + "/config.ini";
    qDebug() << "Saving to:" << configPath;

    QSettings settings("config.ini", QSettings::IniFormat);

    if (mMainPage) {
        if (auto bus = mMainPage->getBusBulk()) {
            bus->saveSettings(settings);
        }
        if (auto mp = mMainPage->getMpBulk()) {
            mp->saveSettings(settings);
        }
        if (auto ip = mMainPage->getIpBulk()) {
            ip->saveSettings(settings);
        }
    }
}

void MainWindow::loadAllSettings() {
    QSettings settings("config.ini", QSettings::IniFormat);

    if (mMainPage) {
        if (auto bus = mMainPage->getBusBulk()) {
            bus->loadSettings(settings);
        }
        if (auto mp = mMainPage->getMpBulk()) {
            mp->loadSettings(settings);
        }
        if (auto ip = mMainPage->getIpBulk()) {
            ip->loadSettings(settings);
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    saveAllSettings();
    QMainWindow::closeEvent(event);
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
