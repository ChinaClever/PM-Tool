#include "mainwid.h"
#include "ui_mainwid.h"
#include "backcolourcom.h"
#include "templateselectdialog.h"
#include <QDebug>

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

void mainWid::on_SelectTemplateBtn_clicked()
{
    TemplateSelectDialog dlg(this);
    if(dlg.exec() == QDialog::Accepted)
    {
        QString pn = dlg.selectedTemplate(); // 获取模板号
        qDebug()<<pn;
    }
}

