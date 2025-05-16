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
    ui->ip_BulkSendWid->addWidget(mIpBulkSend);
}

mp_mainPage::~mp_mainPage()
{
    delete ui;
}
