#include "seripport.h"
#include "ui_seripport.h"
#include "mainPage/mainpage_globals.h"
#include <QIntValidator>
#include <qdebug.h>

serIpPort::serIpPort(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::serIpPort)
{
    ui->setupUi(this);

    serIp = ui->serIp->text();
    port = ui->port->text();

    ui->serIp->setValidator(new QRegExpValidator(QRegExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b")));
    ui->port->setValidator(new QIntValidator(0, 65532,ui->port));

    connect(ui->serIp, &QLineEdit::editingFinished, this, [this](){
        serIp = ui->serIp->text();
    });
    connect(ui->port, &QLineEdit::editingFinished, this, [this](){
        port = ui->port->text();
    });
}

serIpPort::~serIpPort()
{
    delete ui;
}
