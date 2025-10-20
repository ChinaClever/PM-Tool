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
    ui->outletTab->setRowCount(outletPhase);

    setEle(); //更新电能值并显示在界面上
    connect(ui->boxCirTab, &QTableWidget::cellDoubleClicked,
            this, &box_ui::RowEdit);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this,&box_ui::setEle);
   // timer->start(10000);  // 每10秒触发一次
}

void box_ui::timerStart(bool flag)
{
    if(flag){
        setEle();
        timer->start(10000);
    }
    else timer->stop();
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
    setBoxPhaseData();
}

void box_ui::setBoxPhaseData()
{
    QTableWidget* table = ui->boxCirTab;
    int rowCount = table->rowCount();

    for(int r = 0; r < 3; ++r){
        phase[id][r%3][0] = 0; //相电流
        phase[id][r%3][2] = 0; //有功功率
        phase[id][r%3][3] = 0; //无功功率
        phase[id][r%3][4] = 0; //视在功率
        phase[id][r%3][5] = 0; //有功电能
        phase[id][r%3][6] = 0; //无功电能
        phase[id][r%3][7] = 0;
    }

    for (int g = 0; g < outletPhase; ++g) {
        outlet[id][g][0] = 0;  // 有功功率
        outlet[id][g][1] = 0;  // 无功功率
        outlet[id][g][2] = 0;  // 视在功率
        outlet[id][g][4] = 0;  // 无功电能
        outlet[id][g][6] = 0;  // 有功电能
    }

    for (int r = 0; r < rowCount; ++r) {
        // 获取第 r 行第 0 列的状态（如“闭合”或“断开”）
        QTableWidgetItem* statusItem = table->item(r, 0);
        if (!statusItem)
            continue;  // 如果该行没有状态数据，跳过

        QString statusText = statusItem->text().trimmed();

        if (statusText != QStringLiteral("闭合"))
            continue;  // 如果不是“闭合”，不计入统计，跳过

        // 计算该行属于哪个插接箱（按 outletPhase 数均匀分组）
        int outletGroup = getOutletGroup(r, rowCount, outletPhase);

        // 处理第 1 列：相电流
        statusItem = table->item(r, 1);
        double s = statusItem->text().toDouble();
        phase[id][r % 3][0] += s;  // 累加到相电流（按相：A、B、C）

        // 处理第 2 列：有功功率
        statusItem = table->item(r, 2);
        s = statusItem->text().toDouble();
        phase[id][r % 3][2] += s;  // 相有功功率累加
        outlet[id][outletGroup][0] += s;  // 插接箱有功功率累加

        // 处理第 3 列：无功功率
        statusItem = table->item(r, 3);
        s = statusItem->text().toDouble();
        phase[id][r % 3][3] += s;  // 相无功功率累加
        outlet[id][outletGroup][1] += s;  // 插接箱无功功率累加

        // 处理第 4 列：视在功率
        statusItem = table->item(r, 4);
        s = statusItem->text().toDouble();
        phase[id][r % 3][4] += s;  // 相视在功率累加
        outlet[id][outletGroup][2] += s;  // 插接箱视在功率累加

        // 处理第 6 列：有功电能
        statusItem = table->item(r, 6);
        s = statusItem->text().toDouble();
        phase[id][r % 3][5] += s;  // 相有功电能累加
        outlet[id][outletGroup][6] += s;  // 插接箱有功电能累加

        // 处理第 7 列：无功电能
        statusItem = table->item(r, 7);
        s = statusItem->text().toDouble();
        phase[id][r % 3][6] += s;  // 相无功电能累加
        outlet[id][outletGroup][4] += s;  // 插接箱无功电能累加
    }

    for(int i = 0; i < 3; ++i){
        phase[id][i][1] = phase[id][i][4] == 0 ? 0 : phase[id][i][2]/(phase[id][i][4]*1.0);
        phase[id][i][8] = phase[id][i][0]/cirCur;
        //电流谐波
        phase[id][i][7] = specRanNumGgen::get_thd();
    }
    ui->outletTab->setRowCount(outletPhase);
    for (int i = 0; i < outletPhase; ++i) {
        // 功率因数 = 有功功率 / 视在功率
        outlet[id][i][3] = (outlet[id][i][2] == 0)
                               ? 0
                               : outlet[id][i][0] / (outlet[id][i][2] * 1.0);

        // 视在电能 = sqrt(无功电能² + 有功电能²)
        outlet[id][i][5] = sqrtf(
            powf(outlet[id][i][4], 2) + powf(outlet[id][i][6], 2)
            );
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
    for (int r = 0; r < outletPhase; ++r) {
        for (int c = 0; c < 7; ++c) {
            // 转成字符串，保留2位小数
            QString text = QString::number(outlet[id][r][c], 'f', 2);

            QTableWidgetItem* item = new QTableWidgetItem(text);
            ui->outletTab->setItem(r, c, item);
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

BoxData box_ui::generaData()
{
    //box cfg
    {
        auto &u = data.boxCfg;
        u.alarmCount = 5;
        u.iof = 1;
        u.boxVersion = 1;
        u.baudRate = 9600;
        u.beep = 1;
        u.itemType = 1;
        u.workMode = this->id;
        u.loopNum = cirNum;
        u.init(u.loopNum);
        u.boxType = 0;
        QTableWidget* table = ui->boxCirTab;
        for(int r = 0; r < cirNum; ++r){
            QTableWidgetItem* statusItem = table->item(r, 0);
            QString statusText = statusItem->text().trimmed();
            if(r<3)
            u.breakerStatus[r] = (statusText == QStringLiteral("闭合")) ? 1 : 0;
        }
    }

    //loop item list
    {
        auto &u = data.loopItemList;
        u.init(data.boxCfg.loopNum);
        QTableWidget* table = ui->boxCirTab;
        for(int i = 0; i < data.boxCfg.loopNum; ++i){

            u.volValue[i] = phaseVol;
            u.volMax[i] = phaseVol; u.volMin[i] = phaseVol;
            u.volStatus[i] = 0;

            QTableWidgetItem* itm = table->item(i, 1);
            u.curValue[i] = itm->text().toDouble();
            u.curMax[i] = cirCur; u.curMin[i] = 0;
            if(u.curValue[i]>u.curMax[i])u.curStatus[i] = 2;

            itm = table->item(i, 2);
            u.powValue[i] = itm->text().toDouble();
            u.powMax[i] = u.powValue[i] + 1; u.powMin[i] = 0;
            u.powStatus[i] = 0;

            itm = table->item(i, 3);
            u.powReactive[i] = itm->text().toDouble();

            itm = table->item(i, 4);
            u.powApparent[i] = itm->text().toDouble();

            itm = table->item(i, 5);
            u.powerFactor[i] = itm->text().toDouble();

            itm = table->item(i ,6);
            u.eleActive[i] = itm->text().toDouble();

            itm = table->item(i, 7);
            u.eleReactive[i] = itm->text().toDouble();
        }
    }

    //line item list
    {
        auto &u = data.lineItemList;
        u.init(3);
        for(int i = 0; i < 3; ++i){
            u.volValue[i] = phaseVol;
            u.curValue[i] = phase[this->id][i][0];
            u.powerFactor[i] = phase[this->id][i][1];
            u.powActive[i] = phase[this->id][i][2];
            u.powReactive[i] = phase[this->id][i][3];
            u.powApparent[i] = phase[this->id][i][4];
            u.eleActive[i] = phase[this->id][i][5];
            u.eleReactive[i] = phase[this->id][i][6];
            u.curThd[i] = phase[this->id][i][7];
            u.loadRate[i] = phase[this->id][i][8];

        }
    }

    // outlet item list
    {
        auto &u = data.outletItemList;
        u.init(outletPhase);

        for(int i = 0; i < outletPhase; ++i){
            u.powActive[i] = outlet[this->id][i][0];
            u.powReactive[i] = outlet[this->id][i][1];
            u.powApparent[i] = outlet[this->id][i][2];
            u.powerFactor[i] = outlet[this->id][i][3];
            u.eleReactive[i] = outlet[this->id][i][4];
            u.eleApparent[i] = outlet[this->id][i][5];
            u.eleActive[i] = outlet[this->id][i][6];
        }
    }

    //box total data
    {
        auto &u = data.boxTotalData;
        u.powActive = ui->powAct->value();
        u.powApparent = ui->totalPA->value();
        u.powReactive = ui->powReact->value();
        u.powerFactor = ui->totalPf->value();
        u.eleActive = ui->totalEle->value();
        u.eleReactive = ui->totalReacEle->value();
        u.eleApparent = std::sqrt(pow(u.eleActive,2)+pow(u.eleReactive,2));
    }

    return data;
}

BusData box_ui::generaBus()
{
    busdata.init(3);
    //bus cfg
    {
        auto &u = busdata.busCfg;
        //u.curSpecs = 800;
        u.workMode = 1;
        u.beep = 1;
        u.acDc = 0;
        u.boxNum = boxNum;
        u.iof = 1;
        u.isd = 1;
        u.shuntTrip = 1;
        u.breakerStatus = 1;
        u.lspStatus = 1;
        u.busVersion = 2;
        u.itemType = 0;
        u.baudRate = 4;
        u.alarmCount = 5;
    }

    //line item list
    {
        auto &u = busdata.lineItemList;

        for(int i = 0; i < 3; ++i) {
            u.volValue[i] = busPhase[0][i];
            u.curValue[i] = busPhase[1][i];
            u.powerFactor[i] = busPhase[2][i];
            u.powValue[i] = busPhase[3][i];
            u.powReactive[i] = busPhase[4][i];
            u.powApparent[i] = busPhase[5][i];
            u.eleActive[i] = busPhase[6][i];
            u.eleReactive[i] = busPhase[7][i];
            u.volThd[i] = 0;  //电压谐波未计算
            u.curThd[i] = busPhase[8][i]; //电流谐波未计算
            u.loadRate[i] = busPhase[9][i];

            u.volMax[i] = u.volValue[i];
            u.volMin[i] = u.volValue[i];
            if(u.volValue[i] > u.volMax[i])u.volStatus[i] = 2;

            // u.curMax[i] = busdata.busCfg.curSpecs;
            // if(u.curValue[i] > u.curMax[i])u.curStatus[i] = 2;
            u.curMin[i] = 0;

            u.powMax[i] = u.powValue[i];
            u.powMin[i] = 0;
            if(u.powValue[i] > u.powMax[i]) u.powStatus[i] = 0;

            u.volLineValue[i] = std::sqrt(3.0)*u.volValue[i]*1.0;
            u.volLineMax[i] = u.volLineValue[i];
            u.volLineMin[i] = u.volLineValue[i];

        }
    }

    //bus total data  没有数据转到bus_ui上处理
    {
 //       auto &u = busdata.busTotalData;

    }
    return busdata;
}


int box_ui::getOutletGroup(int r, int rowCount, int outletPhase) {
    int groupSize = (rowCount + outletPhase - 1) / outletPhase;  // 等价于 ceil
    int group = r / groupSize;
    if (group >= outletPhase)
        group = outletPhase - 1;
    return group;
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
    int currentRows = table->rowCount();

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



