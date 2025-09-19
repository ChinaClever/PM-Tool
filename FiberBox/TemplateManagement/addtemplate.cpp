#include "addtemplate.h"
#include "ui_addtemplate.h"
#include "backcolourcom.h"
addTemplate::addTemplate(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::addTemplate)
{
    ui->setupUi(this);
        set_background_icon(this,":/image/back.jpg");
}

addTemplate::~addTemplate()
{
    delete ui;
}
