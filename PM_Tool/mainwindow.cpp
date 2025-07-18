#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "databasemanager.h"
#include <QDebug>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mBusbulk(new busBulk(this))
{
    ui->setupUi(this);


    initDataBase(); // === 添加数据库初始化和建表 ===

    intiWid();
    loadAllSettings(); //读写配置文件


}

void MainWindow::initDataBase()
{
    QString appDir = QCoreApplication::applicationDirPath();
    QString dbDir = appDir + "/db";
    QDir dir(dbDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString dbFilePath = dbDir + "/energy_data.db";

    if (!DatabaseManager::instance().init(dbFilePath)) {
        qDebug() << "数据库初始化失败";
    } else {
        qDebug() << "数据库初始化成功";
    }

    if (!DatabaseManager::instance().createThreePhaseEnergyTable()) {
        qDebug() << "创建三相电能表失败";
    } else {
        qDebug() << "三相电能表已创建或已存在";
    }

    if (!DatabaseManager::instance().createSignalPhaseEnergyTable()) {
        qDebug() << "创建单相电能表失败";
    } else {
        qDebug() << "单相电能表已创建或已存在";
    }

    if (!DatabaseManager::instance().createBoxPhaseEnergyTable()) {
        qDebug() << "创建插接箱电能表失败";
    } else {
        qDebug() << "插接箱电能表已创建或已存在";
    }

    if (!DatabaseManager::instance().createOutputBitEnergyTable()) {
        qDebug() << "创建 OutputBitEnergy 表失败";
    } else {
        qDebug() << "OutputBitEnergy 表已创建或已存在";
    }
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
    //qDebug() << "Saving to:" << configPath;

    QSettings settings(configPath, QSettings::IniFormat); // 这里使用完整路径

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

    settings.sync(); // 同步写磁盘
}

void MainWindow::loadAllSettings() {
    QString configPath = QCoreApplication::applicationDirPath() + "/config.ini";

    QSettings settings(configPath, QSettings::IniFormat); // 这里使用完整路径

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
