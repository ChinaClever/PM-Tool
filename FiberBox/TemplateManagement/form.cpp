#include "form.h"
#include "ui_form.h"
#include "backcolourcom.h"
#include "addtemplate.h"
Form::Form(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Form)
{
    ui->setupUi(this);
    set_background_icon(this,":/image/back.jpg");

    QList<QStringList> data = {
        {"260021","SLIM CASSETTE16F LC MTP ULTRA OS2 UNIVERSAL ","A031378AA","MTP-LC 16F","1","通用 UNIVERSAL","OS2","0.50","119*29"},
        {"260022","SLIM CASSETTE16F LC MTP ULTRA OM4 UNIVERSAL ","A031378AA","MTP-LC 16F","2","通用 UNIVERSAL","OM4","0.55","119*29"},

    };

    // 填充表格
    for(const QStringList &rowData : data) {
        int row = ui->tableWidget->rowCount();  // 当前行数
        ui->tableWidget->insertRow(row);        // 插入新行

        for(int col = 0; col < rowData.size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(rowData[col]);
            item->setTextAlignment(Qt::AlignCenter);  // 居中显示
            ui->tableWidget->setItem(row, col, item);
        }
    }
}

Form::~Form()
{
    delete ui;
}

void Form::on_pushButton_clicked()
{
    // 创建对话框实例
    addTemplate dlg(this);

    // 以模态方式显示
    if(dlg.exec() == QDialog::Accepted) {
        // 用户点击了 OK，获取数据

        // TODO: 调用保存函数，例如 insertTemplate(...)
    }
}

