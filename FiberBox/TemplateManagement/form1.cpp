#include "form1.h"
#include "ui_form1.h"
#include "backcolourcom.h"
Form1::Form1(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Form1)
{
    ui->setupUi(this);
    set_background_icon(this,":/image/back.jpg");
}

Form1::~Form1()
{
    delete ui;
}
