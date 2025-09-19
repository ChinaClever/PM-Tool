#include "mainwid.h"
#include "ui_mainwid.h"
#include "backcolourcom.h"
#include "templateselectdialog.h"
#include <QDebug>

mainWid::mainWid(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::mainWid)
    , fiberscan(new FiberScan(this))
    //, temselect(new TemplateSelect(this))
{
    ui->setupUi(this);

    set_background_icon(this,":/image/back.jpg");
    hide();
    ui->tabWidget->setStyleSheet(
        // TabWidget 内容区透明
        "QTabWidget::pane {"
        "background: transparent;"
        "border: none;"
        "}"
        // TabBar 标签透明
        // "QTabBar::tab {"
        // "background: transparent;"
        // "border: none;"
        // "}"
        // "QTabBar::tab:selected {"
        // "background: transparent;"
        // "}"
        // TabWidget整体透明
        "QTabWidget {"
        "background: transparent;"
        "border: none;"
        "}"
        );

    // QVBoxLayout *layout = new QVBoxLayout(ui->groupBox);
    // layout->addWidget(temselect);
    // ui->groupBox->setLayout(layout);
    // ui->groupBox->layout()->setContentsMargins(0, 5, 0, 5);
    fillFiberTable();
}

mainWid::~mainWid()
{
    delete ui;
}

void mainWid::hide()
{
    ui->label_18->hide();      // 配件2编码
    ui->lineEdit_17->hide();
    ui->pushButton_3->hide();

    ui->label_19->hide();      // 配件3编码
    ui->lineEdit_18->hide();
    ui->pushButton_4->hide();

    ui->label_20->hide();      // 配件4编码
    ui->lineEdit_19->hide();
    ui->pushButton_5->hide();
}

void mainWid::fillFiberTable()
{
    QString rawData = R"(Fiber 01: 0.22dB / 0.21dB
Fiber 02: 0.15dB / 0.13dB
Fiber 03: 0.29dB / 0.13dB
Fiber 04: 0.07dB / 0.08dB
Fiber 05: 0.08dB / 0.13dB
Fiber 06: 0.05dB / 0.05dB
Fiber 07: 0.03dB / 0.03dB
Fiber 08: 0.04dB / 0.04dB
Fiber 09: 0.31dB / 0.33dB
Fiber 10: 0.03dB / 0.03dB
Fiber 11: 0.25dB / 0.26dB
Fiber 12: 0.22dB / 0.19dB)";

    // 清空表格
    ui->tableWidgetTop5->clear();
    ui->tableWidgetTop5->setRowCount(0);
    ui->tableWidgetTop5->setColumnCount(4);

    QStringList headers;
    headers << "序号" << "Value (dB)" << "序号" << "Value (dB)";
    ui->tableWidgetTop5->setHorizontalHeaderLabels(headers);
    ui->tableWidgetTop5->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 正则匹配 Fiber 序号和完整 Value（保留 /）
    QRegularExpression re(R"(Fiber\s*(\d+):\s*(.*))");
    QRegularExpressionMatchIterator it = re.globalMatch(rawData);

    QList<QString> fiberNumbers;
    QList<QString> fiberValues;

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        fiberNumbers.append(match.captured(1));
        fiberValues.append(match.captured(2).trimmed()); // 保留完整 Value
    }

    int totalFibers = fiberNumbers.size();
    int half = totalFibers / 2; // 偶数，左/右列平分

    for (int i = 0; i < half; ++i) {
        int row = ui->tableWidgetTop5->rowCount();
        ui->tableWidgetTop5->insertRow(row);

        // 左列
        QTableWidgetItem *leftNum = new QTableWidgetItem("Fiber " + fiberNumbers[i]);
        QTableWidgetItem *leftVal = new QTableWidgetItem(fiberValues[i]);
        leftNum->setTextAlignment(Qt::AlignCenter);
        leftVal->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetTop5->setItem(row, 0, leftNum);
        ui->tableWidgetTop5->setItem(row, 1, leftVal);

        // 右列
        QTableWidgetItem *rightNum = new QTableWidgetItem("Fiber " + fiberNumbers[i + half]);
        QTableWidgetItem *rightVal = new QTableWidgetItem(fiberValues[i + half]);
        rightNum->setTextAlignment(Qt::AlignCenter);
        rightVal->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetTop5->setItem(row, 2, rightNum);
        ui->tableWidgetTop5->setItem(row, 3, rightVal);
    }

    // 设置行高
    for (int row = 0; row < ui->tableWidgetTop5->rowCount(); ++row) {
        ui->tableWidgetTop5->setRowHeight(row, 16);
    }
}


void mainWid::on_pushButton_2_clicked()
{
    fiberscan->setWindowTitle("请手动完整输入扇出线信息");
    if(fiberscan->exec() == QDialog::Accepted){

    }
    else{
        qDebug()<<"Cancel";
    }
}

