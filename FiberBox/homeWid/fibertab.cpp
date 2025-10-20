#include "FiberTab.h"
#include <QTimer>
#include <QDebug>
FiberTab::FiberTab(QWidget *parent)
    : QWidget(parent)
{
    QGridLayout *gridLayout = new QGridLayout(this);

    QFont font;
    font.setPointSize(11);

    labelId = new QLabel("ID：", this);
    labelId->setMinimumHeight(35);
    labelId->setFont(font);
    labelId->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    lineId = new QLineEdit(this);
    lineId->setEnabled(false);
    lineId->setMinimumSize(200, 35);
    lineId->setFont(font);

    labelFiber = new QLabel("光纤数量：", this);
    labelFiber->setFont(font);
    labelFiber->setMinimumHeight(35);
    labelFiber->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    lineFiber = new QLineEdit(this);
    lineFiber->setEnabled(false);
    lineFiber->setMinimumHeight(35);
    lineFiber->setFont(font);

    gridLayout->addWidget(labelId,   0, 0);
    gridLayout->addWidget(lineId,    0, 1);
    gridLayout->addWidget(labelFiber,0, 2);
    gridLayout->addWidget(lineFiber, 0, 3);

    labelWl = new QLabel("波长值：", this);
    labelWl->setFont(font);
    labelWl->setMinimumHeight(35);
    labelWl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    lineWl = new QLineEdit(this);
    lineWl->setEnabled(false);
    lineWl->setMinimumSize(200, 35);
    lineWl->setFont(font);

    labelIL = new QLabel("IL限值：", this);
    labelIL->setFont(font);
    labelIL->setMinimumHeight(35);
    labelIL->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    lineIL = new QLineEdit(this);
    lineIL->setEnabled(false);
    lineIL->setMinimumHeight(35);
    lineIL->setFont(font);

    gridLayout->addWidget(labelWl, 1, 0);
    gridLayout->addWidget(lineWl,  1, 1);
    gridLayout->addWidget(labelIL, 1, 2);
    gridLayout->addWidget(lineIL,  1, 3);

    QGroupBox *groupBox = new QGroupBox("光纤损耗值", this);
    QGridLayout *gbLayout = new QGridLayout(groupBox);
    gbLayout->setContentsMargins(0, 0, 0, 0);
    gbLayout->setHorizontalSpacing(4);

    tableWidget = new QTableWidget(groupBox);
    tableWidget->setEnabled(false);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setStyleSheet(
        "QTableWidget {"
        "   background-color: #f9f9f9;"
        "   border: 1px solid #ccc;"
        "   font-size: 13px;"
        "}"
        "QTableWidget::item:selected {"
        "   background-color: #87cefa;"
        "   color: black;"
        "   font-weight: bold;"
        "}"
        );

    tableWidget->setColumnCount(4);
    QStringList headers;
    headers << "序号" << "Value (dB)" << "序号" << "Value (dB)";
    tableWidget->setHorizontalHeaderLabels(headers);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    gbLayout->addWidget(tableWidget, 0, 0);
    gridLayout->addWidget(groupBox, 2, 0, 1, 4);
}

void FiberTab::fillFiberTable(const QString &rawData)
{
    tableWidget->setRowCount(0);  // 清空旧数据

    QRegularExpression re(R"(F(\d+):\s*(.*))");
    QRegularExpressionMatchIterator it = re.globalMatch(rawData);

    QList<QString> fiberNumbers;
    QList<QString> fiberValues;

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        fiberNumbers.append(match.captured(1));
        fiberValues.append(match.captured(2).trimmed());
    }

    int totalFibers = fiberNumbers.size();
    int half = (totalFibers + 1) / 2; // 支持奇数

    for (int i = 0; i < half; ++i) {
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);

        // 左列
        QTableWidgetItem *leftNum = new QTableWidgetItem("F" + fiberNumbers[i]);
        QTableWidgetItem *leftVal = new QTableWidgetItem(fiberValues[i]);
        leftNum->setTextAlignment(Qt::AlignCenter);
        leftVal->setTextAlignment(Qt::AlignCenter);
        tableWidget->setItem(row, 0, leftNum);
        tableWidget->setItem(row, 1, leftVal);

        // 右列
        if (i + half < totalFibers) {
            QTableWidgetItem *rightNum = new QTableWidgetItem("F" + fiberNumbers[i + half]);
            QTableWidgetItem *rightVal = new QTableWidgetItem(fiberValues[i + half]);
            rightNum->setTextAlignment(Qt::AlignCenter);
            rightVal->setTextAlignment(Qt::AlignCenter);
            tableWidget->setItem(row, 2, rightNum);
            tableWidget->setItem(row, 3, rightVal);
        }
    }

    tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // **直接设置行高，不用延迟**
    int rowCount = tableWidget->rowCount();
    if (rowCount > 0) {
        int availableHeight = tableWidget->viewport()->height();
        if (availableHeight > 0) {
            int rowHeight = availableHeight / rowCount;
            for (int row = 0; row < rowCount; ++row)
                tableWidget->setRowHeight(row, rowHeight);
        }
    }
}



void FiberTab::setInfo(const QString &id,int fiberCount,
             const QString &wavelength,const QString &ilLimit)
{
    lineId->setText(id);
    lineFiber->setText(QString::number(fiberCount));
    lineWl->setText(wavelength);
    lineIL->setText(ilLimit);
}
