#include "temmainwid.h"
#include "ui_temmainwid.h"
#include "dbtem.h"
#include "backcolour/backcolourcom.h"
temMainwid::temMainwid(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::temMainwid)
{
    ui->setupUi(this);
    set_background_icon(this,":/image/back.jpg");
    mBtnBar = new TemBtnBar;
    mDbTable = new SqlTableWid(ui->groupBox);
    mDbTable->initWid(DbTem::build(), mBtnBar);
}

temMainwid::~temMainwid()
{
    delete ui;
}
