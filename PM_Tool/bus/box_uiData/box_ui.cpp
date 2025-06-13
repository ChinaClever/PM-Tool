#include "box_ui.h"
#include "ui_box_ui.h"
#include "specRanNumGgen.h"
#include "bus_globals/bus_globals.h"
#include "data_cal/data_cal.h"
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QElapsedTimer>
#include <QDebug>
#include <QTimer>
#include <math.h>
box_ui::box_ui(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::box_ui)
{

    ui->setupUi(this);
    ui->tabWidget->tabBar()->hide();
    //setEle(); //更新电能值并显示在界面上
    connect(ui->boxCirTab, &QTableWidget::cellDoubleClicked,
            this, &box_ui::RowEdit);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this,&box_ui::setEle);
    timer->start(10000);  // 每10秒触发一次
}


void box_ui::setEle()  //计算电能并更新outlet以及相数据
{

    QTableWidget* table = ui->boxCirTab;
    int rowCount = table->rowCount();
    const double seconds = 10.0;  // 定时器间隔 10 秒

    for (int r = 0; r < rowCount; ++r) {

        QTableWidgetItem* statusItem = table->item(r, 0);
        if (!statusItem)
            continue;

        double V = phaseVol;
        double I;
        double Pf;
        double activePower = 0.0;
        double reactivePower = 0.0;
        double App = 0.0;
        {
            QTableWidget* table = ui->boxCirTab;
            if (table->item(r, 1))
                I = table->item(r, 1)->data(Qt::EditRole).toDouble();
            else
                I = 0.0;

            if (table->item(r, 5))
                Pf = table->item(r, 5)->data(Qt::EditRole).toDouble();
            else
                Pf = 0.0;

            activePower = Pf * V * I /1000.0;
            App = V*I / 1000.0;
            reactivePower = sqrtf(App*App-activePower*activePower);
            QTableWidgetItem* activePowerItem = table->item(r, 2);
            if (!activePowerItem) {
                activePowerItem = new QTableWidgetItem();
                table->setItem(r, 2, activePowerItem);
            }
            activePowerItem->setText(QString::number(activePower, 'f', 2)); // 保留两位小数

            // 无功功率显示（第3列）
            QTableWidgetItem* reactivePowerItem = table->item(r, 3);
            if (!reactivePowerItem) {
                reactivePowerItem = new QTableWidgetItem();
                table->setItem(r, 3, reactivePowerItem);
            }
            reactivePowerItem->setText(QString::number(reactivePower, 'f', 2));
            reactivePowerItem = table->item(r, 4);
            reactivePowerItem->setText(QString::number(App, 'f', 2));
        }

        // 先判断第0列是否为"闭合"，如果不是则跳过该行
        QString statusText = statusItem->text().trimmed();
        qDebug()<<statusText;

        if (statusText != QStringLiteral("闭合"))
            continue;

        // 更新电能数组
        cirEle[id][r] += activePower * seconds / 3600.0;
        cirReacEle[id][r] += reactivePower * seconds / 3600.0;

        // 更新有功电能显示（第6列）
        QTableWidgetItem* activeEleItem = table->item(r, 6);
        if (!activeEleItem) {
            activeEleItem = new QTableWidgetItem();
            activeEleItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            table->setItem(r, 6, activeEleItem);
        }
        activeEleItem->setText(QString::number(cirEle[id][r], 'f', 4));

        // 更新无功电能显示（第7列）
        QTableWidgetItem* reactiveEleItem = table->item(r, 7);
        if (!reactiveEleItem) {
            reactiveEleItem = new QTableWidgetItem();
            reactiveEleItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            table->setItem(r, 7, reactiveEleItem);
        }
        reactiveEleItem->setText(QString::number(cirReacEle[id][r], 'f', 4));
    }

    qDebug() << "电能数据已更新。";
    setBoxPhaseData();
}

void box_ui::setBoxPhaseData()
{
    QTableWidget* table = ui->boxCirTab;
    int rowCount = table->rowCount();

    for (int r = 0; r < rowCount; ++r){

        QTableWidgetItem* statusItem = table->item(r, 0);
        if (!statusItem)
            continue;
        QString statusText = statusItem->text().trimmed();
        qDebug()<<statusText;

        if (statusText != QStringLiteral("闭合"))
            continue;

        statusItem = table->item(r, 1);
        double s = statusItem->text().toDouble();
        phase[id][r%3][0] += s;  //相电流

        statusItem = table->item(r, 2);
        s = statusItem->text().toDouble();
        phase[id][r%3][2] += s; //有功功率

        statusItem = table->item(r, 3);
        s = statusItem->text().toDouble();
        phase[id][r%3][3] += s; //无功功率

        statusItem = table->item(r, 4);
        s = statusItem->text().toDouble();
        phase[id][r%3][4] += s; //视在功率

        statusItem = table->item(r, 6);
        s = statusItem->text().toDouble();
        phase[id][r%3][5] += s; //有功电能

        statusItem = table->item(r, 7);
        s = statusItem->text().toDouble();
        phase[id][r%3][6] += s; //无功电能

    }
    for(int i = 0; i < 3; ++i){
        phase[id][i][1] = phase[id][i][4] == 0 ? 0 : phase[id][i][2]/(phase[id][i][4]*1.0);
        phase[id][i][8] = phase[id][i][0]/cirCur;
        //电流谐波
        phase[id][i][7] = 0;
    }
    ui->boxPhaseTab->setRowCount(3);
    ui->boxPhaseTab->setColumnCount(9);

    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 9; ++c) {
            // 转成字符串，保留2位小数
            QString text = QString::number(phase[id][r][c], 'f', 2);

            QTableWidgetItem* item = new QTableWidgetItem(text);
            ui->boxPhaseTab->setItem(r, c, item);
        }
    }

//赋值插接箱统计数据
    ui->powAct->setValue(phase[id][0][2] + phase[id][1][2] + phase[id][2][2]);
    ui->powReact->setValue(phase[id][0][3] + phase[id][1][3] + phase[id][2][3]);
    ui->totalPA->setValue(phase[id][0][4] + phase[id][1][4] + phase[id][2][4]);
    ui->totalReacEle->setValue(phase[id][0][6] + phase[id][1][6] + phase[id][2][6]);
    ui->totalEle->setValue(phase[id][0][5] + phase[id][1][5] + phase[id][2][5]);
    ui->totalPf->setValue(ui->totalPA->value() == 0 ? 0 : ui->powAct->value()/ui->totalPA->value());
}

void box_ui::setBoxoutData()
{
    QTableWidget* table = ui->boxCirTab;
    int rowCount = table->rowCount();
    QVector<QVector<double>>out(3, QVector<double>(7, 0.0));
    for (int r = 0; r < rowCount; ++r){

    }
}

void box_ui::RowEdit(int row)   //双击编辑行数据
{
    // 安全检查
    if(row < 0 || row >= ui->boxCirTab->rowCount()) return;

    // 创建对话框
    QDialog dialog(this);
    dialog.setFixedSize(300, 200);
    dialog.setWindowTitle(QString("编辑回路数据参数 - 第%1行").arg(row+1));
    QFormLayout layout(&dialog);

    // 定义要显示的参数列（索引、名称、验证规则）
    const QVector<QPair<int, QString>> params = {
        {0, "开关状态"},  // 第0列
        {1, "电流(A)"},   // 第1列
        {5, "功率因数"},  // 第5列
    };

    // 为每个参数创建编辑控件
    QVector<QWidget*> editors;
    QCheckBox* switchCheckBox = nullptr;

    for(const auto& param : params) {
        int col = param.first;

        if(col == 0) { // 开关状态列
            switchCheckBox = new QCheckBox(&dialog);

            // 获取单元格文本
            QString statusText = ui->boxCirTab->item(row, col) ?
                                     ui->boxCirTab->item(row, col)->text() : "";

            // 精确匹配文本状态
            bool isChecked = (statusText == "闭合"); // 只有明确是"闭合"才勾选
            switchCheckBox->setChecked(isChecked);

            layout.addRow(param.second, switchCheckBox);
        }
        else {
            // 使用QDoubleSpinBox替代QLineEdit以获得更好的数值控制
            QDoubleSpinBox* spinBox = new QDoubleSpinBox(&dialog);
            spinBox->setDecimals(2);
            spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons); // 隐藏微调按钮

            // 设置范围和当前值
            double defaultValue = 1.0;
            if(col == 1) { // 电流列
                spinBox->setRange(0, 63);
                defaultValue = 1.0;
                spinBox->setSuffix(" A");
            }
            else if(col == 5) { // 功率因数列
                spinBox->setRange(0.2, 1);
                defaultValue = 0.3;

            }

            if(QTableWidgetItem* item = ui->boxCirTab->item(row, col)) {
                spinBox->setValue(item->text().toDouble());
            } else {
                spinBox->setValue(defaultValue);
            }

            layout.addRow(param.second, spinBox);
            editors.append(spinBox);
        }
    }

    // 添加对话框按钮
    QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout.addRow(&buttons);

    // 信号连接
    connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    // 显示对话框并处理结果
    if(dialog.exec() == QDialog::Accepted) {
        // 更新开关状态
        if(switchCheckBox) {
            QTableWidgetItem* item = ui->boxCirTab->item(row, 0);
            if(!item) {
                item = new QTableWidgetItem();
                ui->boxCirTab->setItem(row, 0, item);
            }
            item->setText(switchCheckBox->isChecked() ? "闭合" : "断开");
        }

        // 更新其他参数
        for(int i = 0; i < editors.size(); ++i) {
            int col = params[i+1].first; // +1因为跳过了开关状态
            QDoubleSpinBox* spinBox = qobject_cast<QDoubleSpinBox*>(editors[i]);

            QTableWidgetItem* item = ui->boxCirTab->item(row, col);
            if(!item) {
                item = new QTableWidgetItem();
                ui->boxCirTab->setItem(row, col, item);
            }
            item->setText(QString::number(spinBox->value(), 'f', 2));
        }

        // 触发表格更新信号
        emit ui->boxCirTab->itemChanged(ui->boxCirTab->item(row, 0));
       // setBitPower(row);
    }
}

void box_ui::CntChanged(int targetRows)
{
    QTableWidget* table = ui->boxCirTab;

    if (!table) {
        qDebug() << "表格不存在！";
        return;
    }

    int currentSelectedRow = table->currentRow();
    qDebug() << "当前选中行: " << currentSelectedRow;

    int currentRows = table->rowCount();
    qDebug() << "当前行数:" << currentRows;

    // 保存现有行的电能数据
    for (int i = 0; i < qMin(currentRows, 9); ++i) {
        QTableWidgetItem* item1 = table->item(i, 6);  // 有功电能
        QTableWidgetItem* item2 = table->item(i, 7);  // 无功电能

        if (item1)
            cirEle[id][i] = item1->text().toDouble();
        if (item2)
            cirReacEle[id][i] = item2->text().toDouble();
    }

    // 调整行数
    table->setRowCount(targetRows);

    // 初始化或恢复数据
    for (int r = 0; r < targetRows; ++r) {
        // 断路器列
        if (!table->item(r, 0)) {
            QTableWidgetItem* switchItem = new QTableWidgetItem("闭合");
            switchItem->setFlags(switchItem->flags() & ~Qt::ItemIsEditable);
            switchItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            table->setItem(r, 0, switchItem);
        }

        // 电流列
        if (!table->item(r, 1)) {
            QTableWidgetItem* currentItem = new QTableWidgetItem();
            double val = specRanNumGgen::getrandom(cirCur) * 100;
            //qDebug()<<val<<' '<<r;
            currentItem->setData(Qt::EditRole, val);
            currentItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            currentItem->setData(Qt::UserRole, QVariant(0.0));
            table->setItem(r, 1, currentItem);
        }

        // 其他列
        for (int c = 2; c < table->columnCount(); ++c) {
            if (!table->item(r, c)) {
                QTableWidgetItem* item = new QTableWidgetItem();
                item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

                if (c == 5) {
                    double pfValue = specRanNumGgen::get_power_factor_precise();

                    item->setData(Qt::EditRole, pfValue);

                }

                else if (c == 6) {
                    item->setData(Qt::EditRole, cirEle[id][r]);  // 恢复有功电能
                }
                else if (c == 7) {
                    item->setData(Qt::EditRole, cirReacEle[id][r]);  // 恢复无功电能
                }
                table->setItem(r, c, item);
            }
            else {
                // 如果已有单元格（可能是缩减行数后保留的），也赋值以保证一致性
                if (c == 6) {
                    table->item(r, c)->setData(Qt::EditRole, cirEle[id][r]);
                }
                else if (c == 7) {
                    table->item(r, c)->setData(Qt::EditRole, cirReacEle[id][r]);
                }
            }
        }
        setcirPower(r);
    }

    // 恢复选中行
    if (targetRows > 0) {
        int rowToSelect = qBound(0, currentSelectedRow, targetRows - 1);
        table->setCurrentCell(rowToSelect, 0);
    }


}

void box_ui::setcirPower(const int row)
{
    QTableWidget* table = ui->boxCirTab;

    QTableWidgetItem* pf = new QTableWidgetItem(); //有功功率
    QTableWidgetItem* rePf = new QTableWidgetItem();//无功功率
    QTableWidgetItem* aPf = new QTableWidgetItem(); //视在功率

    double V = phaseVol;
    double I = table->item(row, 1)->data(Qt::EditRole).toDouble();
    double Pf = table->item(row, 5)->data(Qt::EditRole).toDouble();
    double PowFat = data_cal::active_powerCal(V,I,Pf);
    double AFat = data_cal::apparent_powerCal(V,I);

    pf->setData(Qt::EditRole, qRound(PowFat * 1000) / 1000.0);
    aPf->setData(Qt::EditRole, qRound(AFat * 1000) / 1000.0);
    rePf->setData(Qt::EditRole,qRound(data_cal::reactive_powerCal(PowFat,AFat)*1000)/1000.0);

    pf->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter); // 右对齐
    aPf->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter); // 右对齐
    rePf->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter); // 右对齐

    pf->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable); // 禁止直接编辑
    aPf->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable); // 禁止直接编辑
    rePf->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable); // 禁止直接编辑

    table->setItem(row, 2, pf);
    table->setItem(row,3,rePf);
    table->setItem(row,4,aPf);
}

void box_ui::setid(int x)
{
    id = x;
}

box_ui::~box_ui()
{
    delete ui;
}
