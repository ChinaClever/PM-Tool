#include "mp_mainpage.h"
#include "ui_mp_mainpage.h"
#include "backcolourcom.h"

mp_mainPage::mp_mainPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::mp_mainPage)
{
    ui->setupUi(this);
    set_background_icon(this,":/image/box_back.jpg");

    mIpBulkSend = new ip_BulkSend();
    mMpBulkSend = new mp_bulksend();
    mserIpPort = new serIpPort();
    mBusBulk = new busBulk();

    ui->ip_BulkSendWid->addWidget(mIpBulkSend);
    ui->mp_bulksendWid->addWidget(mMpBulkSend);
    ui->serSetWid->addWidget(mserIpPort);
    ui->bus_bulksendWid->addWidget(mBusBulk);

    mserIpPort->setSubModules(mIpBulkSend, mMpBulkSend, mBusBulk);
}

mp_mainPage::~mp_mainPage()
{
    delete ui;
}
