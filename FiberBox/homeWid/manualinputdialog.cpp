// ManualInputDialog.cpp
#include "ManualInputDialog.h"
#include "ui_ManualInputDialog.h"
#include <QMessageBox>

ManualInputDialog::ManualInputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ManualInputDialog)
{
    ui->setupUi(this);

    // 默认设置波长选项
    ui->comboWave->addItem("1310nm/1550nm");
    ui->comboWave->addItem("850nm/1300nm");


    // 表格初始化 3 列：Fiber | λ1 | λ2
    ui->tableWidget->setColumnCount(3);
    QStringList headers = {"Fiber", "λ1", "λ2"};
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

    // 默认纤芯数量 1
    ui->spinFiberCount->setMinimum(1);
    ui->spinFiberCount->setMaximum(64);
    ui->spinFiberCount->setValue(1);

    connect(ui->spinFiberCount, SIGNAL(valueChanged(int)), this, SLOT(on_spinFiberCount_valueChanged(int)));

    adjustTableRows(ui->spinFiberCount->value());
}

ManualInputDialog::~ManualInputDialog()
{
    delete ui;
}

void ManualInputDialog::setTemplateInfo(const QString &fanoutPn, const int &wave, int fiberCount)
{
    fanoutPN = fanoutPn;

    ui->lineEditID->setText(fanoutPN);
    ui->spinFiberCount->setValue(fiberCount);
    ui->comboWave->setCurrentIndex(wave);

    adjustTableRows(fiberCount);
}

void ManualInputDialog::adjustTableRows(int rowCount)
{
    ui->tableWidget->setRowCount(rowCount);
    for (int i = 0; i < rowCount; ++i) {
        // 第一列 Fiber 名称
        QTableWidgetItem *item0 = ui->tableWidget->item(i,0);
        if (!item0) { item0 = new QTableWidgetItem(QString("F%1").arg(i+1,2,10,QChar('0'))); ui->tableWidget->setItem(i,0,item0);}
        item0->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

        // λ1
        QTableWidgetItem *item1 = ui->tableWidget->item(i,1);
        if (!item1) { item1 = new QTableWidgetItem("0.00"); ui->tableWidget->setItem(i,1,item1);}

        // λ2
        QTableWidgetItem *item2 = ui->tableWidget->item(i,2);
        if (!item2) { item2 = new QTableWidgetItem("0.00"); ui->tableWidget->setItem(i,2,item2);}
    }
}

void ManualInputDialog::on_spinFiberCount_valueChanged(int count)
{
    adjustTableRows(count);
}

void ManualInputDialog::on_btnOK_clicked()
{
    if (ui->lineEditSerial->text().isEmpty()) {
        QMessageBox::warning(this, "提示", "序列号不能为空！");
        return;
    }

    if (ui->lineEditBatch->text().isEmpty()) {
        QMessageBox::warning(this, "提示", "批次不能为空！");
        return;
    }

    // 拼接光纤信息
    QStringList fiberList;
    int rows = ui->tableWidget->rowCount();
    for (int i = 0; i < rows; ++i) {
        QString f = ui->tableWidget->item(i,0)->text();  // F01, F02...
        QString l1 = ui->tableWidget->item(i,1)->text();
        QString l2 = ui->tableWidget->item(i,2)->text();
        fiberList.append(QString("%1:%2/%3").arg(f).arg(l1).arg(l2));
    }

    // 拼接完整扫码字符串（状态固定 PASS）
    m_scanCode = QString("%1;%2;%3;%4;<%5dB:PASS;%6")
                     .arg(ui->lineEditID->text())            // ID
                     .arg(ui->lineEditBatch->text())         // Batch
                     .arg(ui->lineEditSerial->text())        // Serial
                     .arg(ui->comboWave->currentText())      // Wave
                     .arg(ui->doubleSpinLimit->value())      // Limit
                     .arg(fiberList.join(";"));              // 光纤列表

    accept();
}



void ManualInputDialog::on_btnCancel_clicked()
{
    reject();
}

QString ManualInputDialog::getScanCode() const
{
    return m_scanCode;
}
