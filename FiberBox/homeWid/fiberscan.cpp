#include "fiberscan.h"
#include "ui_fiberscan.h"
#include "backcolourcom.h"
FiberScan::FiberScan(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FiberScan)
{
    ui->setupUi(this);
    set_background_icon(this,":/image/back.jpg");


    QStringList headers;
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setRowCount(12);
    for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
        QTableWidgetItem *item = new QTableWidgetItem(QString("光纤 %1").arg(i + 1));
        item->setTextAlignment(Qt::AlignCenter); // 居中
        ui->tableWidget->setItem(i, 0, item);    // 第一列
    }

}

FiberScan::~FiberScan()
{
    delete ui;
}
