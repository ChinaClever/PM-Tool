#include "seripport.h"
#include "ui_seripport.h"
#include "mainPage/mainpage_globals.h"
#include <QIntValidator>
#include <qdebug.h>

serIpPort::serIpPort(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::serIpPort)
{
    ui->setupUi(this);



    serIp = ui->serIp1->text();
    port = ui->port1->text();
    BusPort = ui->BusPort1->text();

    serIp2 = ui->serIp2->text();
    port2 = ui->port2->text();
    BusPort2 = ui->BusPort2->text();

    serIp3 = ui->serIp3->text();
    port3 = ui->port3->text();
    BusPort3 = ui->BusPort3->text();

    ui->serIp1->setValidator(new QRegExpValidator(QRegExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b")));
    ui->port1->setValidator(new QIntValidator(0, 65532,ui->port1));
    ui->BusPort1->setValidator(new QIntValidator(0, 65532,ui->BusPort1));
    ui->serIp2->setValidator(new QRegExpValidator(QRegExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b")));
    ui->port2->setValidator(new QIntValidator(0, 65532,ui->port2));
    ui->BusPort2->setValidator(new QIntValidator(0, 65532,ui->BusPort2));
    ui->serIp3->setValidator(new QRegExpValidator(QRegExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b")));
    ui->port3->setValidator(new QIntValidator(0, 65532,ui->port3));
    ui->BusPort3->setValidator(new QIntValidator(0, 65532,ui->BusPort3));

    connect(ui->serIp1, &QLineEdit::editingFinished, this, [this](){
        serIp = ui->serIp1->text();
    });
    connect(ui->port1, &QLineEdit::editingFinished, this, [this](){
        port = ui->port1->text();
    });
    connect(ui->BusPort1, &QLineEdit::editingFinished, this, [this](){
        BusPort = ui->BusPort1->text();
    });

    connect(ui->serIp2, &QLineEdit::editingFinished, this, [this](){
        serIp2 = ui->serIp1->text();
    });
    connect(ui->port2, &QLineEdit::editingFinished, this, [this](){
        port2 = ui->port2->text();
    });
    connect(ui->BusPort2, &QLineEdit::editingFinished, this, [this](){
        BusPort2 = ui->BusPort1->text();
    });

    connect(ui->serIp3, &QLineEdit::editingFinished, this, [this](){
        serIp3 = ui->serIp1->text();
    });
    connect(ui->port3, &QLineEdit::editingFinished, this, [this](){
        port3 = ui->port3->text();
    });
    connect(ui->BusPort3, &QLineEdit::editingFinished, this, [this](){
        BusPort3 = ui->BusPort3->text();
    });
}

serIpPort::~serIpPort()
{
    delete ui;
}

void serIpPort::on_sendAllBtn_clicked()
{
    bool start = (ui->sendAllBtn->text() == "全部发送");

    if (start) {
        ui->sendAllBtn->setText("停止发送");
    } else {
        ui->sendAllBtn->setText("全部发送");
    }
     mIpBulkSend->triggerToggleSend(start);
     mMpBulkSend->triggerToggleSend(start);
     mBusBulk->triggerToggleSend(start);
}

// seripport.cpp

void serIpPort::setSubModules(ip_BulkSend* ip, mp_bulksend* mp, busBulk* bus)
{
    mIpBulkSend = ip;
    mMpBulkSend = mp;
    mBusBulk = bus;
}

