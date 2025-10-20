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
    //testDbTemInsert();
    //testDbLogsInsert();
}

void MainWindow::init()
{
    navigation = new navarwid(ui->barWid);
    mainpage = new mainWid(this);

    temMain = new temMainwid(this);
    mLog    = new LogMainWid(this);
    mSetup  = new Setup_MainWid(this);

    ui->stackedWidget->addWidget(mainpage);
    ui->stackedWidget->addWidget(temMain);
    ui->stackedWidget->addWidget(mLog);
    ui->stackedWidget->addWidget(mSetup);
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

#include "dblogs.h"
void MainWindow::testDbLogsInsert()
{
    // 构造日志数据
    sFiberLogItem log;
    // log.date = QDate::currentDate().toString("yyyy-MM-dd");
    // log.time = QTime::currentTime().toString("HH:mm:ss");
    log.boxId = "BOX-001";
    log.PN = "260021";
    log.fanoutPn = "A018497AA";
    log.description = "SLIM CASSETTE16F LC-MTP OS2 UNIVERSAL";
    log.fanoutCount = 4;
    log.waveType = "SM";
    log.limitIL = 0.5;
    log.qrContent = "QR-TEST";

    log.seq1 = "S1"; log.qr1 = "QR1";
    log.seq2 = "S2"; log.qr2 = "QR2";
    log.seq3 = "S3"; log.qr3 = "QR3";
    log.seq4 = "S4"; log.qr4 = "QR4";

    // 获取日志单例并插入
    auto db = DbLogs::instance();
    if (db->insertItem(log)) {
        qDebug() << "日志插入成功, ID =" << log.id;
    } else {
        qDebug() << "日志插入失败";
    }

    // 查询所有日志并打印
    QVector<sFiberLogItem> logs = db->allItems();
    for (const auto &l : logs) {
        qDebug() << "--------------------------------";
        qDebug() << "ID:" << l.id;
        qDebug() << "日期:" << l.date << "时间:" << l.time;
        qDebug() << "光纤盒ID:" << l.boxId;
        qDebug() << "成品编号:" << l.PN;
        qDebug() << "Fanout PN:" << l.fanoutPn;
        qDebug() << "描述:" << l.description;
        qDebug() << "扇出线ID:" << l.fanoutCount;
        qDebug() << "波类型:" << l.waveType;
        qDebug() << "最大IL:" << l.limitIL;
        qDebug() << "二维码:" << l.qrContent;
        qDebug() << "扇出线序列号/二维码: "
                 << l.seq1 << l.qr1
                 << l.seq2 << l.qr2
                 << l.seq3 << l.qr3
                 << l.seq4 << l.qr4;
        qDebug() << "--------------------------------";
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::navBarSlot(int id)
{
    ui->stackedWidget->setCurrentIndex(id);
}
