#include "mainwid.h"
#include "ui_mainwid.h"
#include "backcolourcom.h"
mainWid::mainWid(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::mainWid)
{
    ui->setupUi(this);
    set_background_icon(this,":/image/back.jpg");
}

mainWid::~mainWid()
{
    delete ui;
}
