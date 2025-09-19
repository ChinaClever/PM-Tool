#include "forrm.h"
#include "ui_forrm.h"
#include "backcolourcom.h"
Forrm::Forrm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Forrm)
{
    ui->setupUi(this);
    set_background_icon(this,":/image/back.jpg");
}

Forrm::~Forrm()
{
    delete ui;
}
