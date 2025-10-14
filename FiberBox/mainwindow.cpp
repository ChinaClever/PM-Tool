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
    testDbTemInsert();
}

void MainWindow::init()
{
    navigation = new navarwid(ui->barWid);
    mainpage = new mainWid(this);

    temMain = new temMainwid(this);

    ui->stackedWidget->addWidget(mainpage);
    ui->stackedWidget->addWidget(temMain);
}

#include "dbtem.h"
void MainWindow::testDbTemInsert()
{
    // sTemItem testItem;
    // testItem.date = QDate::currentDate().toString("yyyy-MM-dd");
    // testItem.time = QTime::currentTime().toString("HH:mm:ss");
    // testItem.data.PN = "260013";
    // testItem.data.description = "SLIM CASSETTE16F LC MTP ULTRA OS2 UNIVERSAL";
    // testItem.data.FanoutPn = "A031378AA";
    // testItem.data.FanCount = 2;
    // testItem.data.limit = 0.5;
    // testItem.data.info.iface = InterfaceType::MTP_LC;
    // testItem.data.info.count = FiberCountType::F16;
    // testItem.data.info.mode = FiberMode::MM;
    // testItem.data.info.spec = FiberSpec::OS2;
    // testItem.data.info.polarity = Polarity::UNIVERSAL;
    // testItem.data.qrTemplate = "Template C";
    // testItem.data.labelTemplate = "Template 1";
    // testItem.data.lambda = {850, 1300};

    // if(DbTem::build()->insertItem(testItem)){
    //     qDebug() << "插入成功, ID =" << testItem.id;
    // } else {
    //     qDebug() << "插入失败";
    // }


    // auto db = DbTem::build();
    // sTemItem item;

    // if (db->getItemByPN("TEST002", item)) {
    //     qDebug() << " 找到模板：";
    //     qDebug() << "--------------------------------";
    //     qDebug() << "ID:" << item.id;
    //     qDebug() << "日期:" << item.date;
    //     qDebug() << "时间:" << item.time;
    //     qDebug() << "Cassette PN:" << item.data.PN;
    //     qDebug() << "Description:" << item.data.description;
    //     qDebug() << "Polarity:" << polarityToString(item.data.info.polarity);
    //     qDebug() << "接口类型:" << ifaceToString(item.data.info.iface);
    //     qDebug() << "光纤数:" << countToString(item.data.info.count);
    //     qDebug() << "二维码模板:" << item.data.qrTemplate;
    //     qDebug() << "Fanouts PN:" << item.data.FanoutPn;
    //     qDebug() << "Fanout 数量:" << item.data.FanCount;
    //     qDebug() << "Label 模板:" << item.data.labelTemplate;
    //     qDebug() << "λ1:" << item.data.lambda.first;
    //     qDebug() << "λ2:" << item.data.lambda.second;
    //     qDebug() << "IL max:" << item.data.limit;
    //     qDebug() << "模式(SM/MM):" << modeToString(item.data.info.mode);
    //     qDebug() << "类型:" << specToString(item.data.info.spec);
    //     qDebug() << "--------------------------------";
    // } else {
    //     qDebug() << " 未找到该PN模板！";
    // }
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
