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



    serIp = ui->serIp->text();
    port = ui->port->text();

    ui->serIp->setValidator(new QRegExpValidator(QRegExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b")));
    ui->port->setValidator(new QIntValidator(0, 65532,ui->port));
    ui->BusPort->setValidator(new QIntValidator(0, 65532,ui->BusPort));

    connect(ui->serIp, &QLineEdit::editingFinished, this, [this](){
        serIp = ui->serIp->text();
    });
    connect(ui->port, &QLineEdit::editingFinished, this, [this](){
        port = ui->port->text();
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

