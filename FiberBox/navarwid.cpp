#include "navarwid.h"
#include "ui_navarwid.h"
#include "backcolourcom.h"
navarwid::navarwid(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::navarwid)
{
    ui->setupUi(this);
    QGridLayout *gridLayout = new QGridLayout(parent);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->addWidget(this);
    set_background_icon(this,":/image/title_back.jpg");
}

navarwid::~navarwid()
{
    delete ui;
}

void navarwid::on_logBtn_clicked()
{
    emit navBarSig(2);
}


void navarwid::on_setBtn_clicked()
{
    emit navBarSig(3);
}


void navarwid::on_templateBtn_clicked()
{
    emit navBarSig(1);
}


void navarwid::on_MainWid_clicked()
{
    emit navBarSig(0);
}

