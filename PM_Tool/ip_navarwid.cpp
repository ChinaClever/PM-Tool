#include "ip_navarwid.h"
#include "ui_ip_navarwid.h"
#include "backcolourcom.h"
#include <QDebug>

IP_NavarWid::IP_NavarWid(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::IP_NavarWid)
{
    ui->setupUi(this);

    set_background_icon(this,":/image/title_back.jpg");

}

IP_NavarWid::~IP_NavarWid()
{
    delete ui;
}

void IP_NavarWid::on_coreBtn_clicked()
{
    emit navBarSig(0);
}

void IP_NavarWid::on_generBtn_clicked()
{

}

void IP_NavarWid::on_mproBtn_clicked()
{

}


void IP_NavarWid::on_ipBtn_clicked()
{
    emit navBarSig(1);
}


void IP_NavarWid::on_setBtn_clicked()
{

}



