#include "mp_data.h"
#include "ui_mp_data.h"
#include "backcolourcom.h"
#include "specrannumggen.h"
#include "data_cal/data_cal.h"
#include <QList>
#include <QElapsedTimer>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QDoubleValidator>
#include <QTimer>
#include <qDebug>
#include <QDateTime>
#include "mp_globals.h"
#include <QUdpSocket>
#include <QJsonDocument>
#include <QThread>

mp_data::mp_data(QWidget *parent)
    : QWidget(parent)
    ,mTojson(new mp_toJson(this))
    , ui(new Ui::mp_data)
{
    ui->setupUi(this);
    m_systemData = PowerSystemData();
    m_systemData.pduData.envData.initialize(4);


    QString configPath = QCoreApplication::applicationDirPath() + "/config.ini";
    QSettings settings(configPath, QSettings::IniFormat);

    loadSettings(settings);
    inti();
    seriesinti();
    m_DglobalUpdateTimer = new QTimer(this);
    m_DglobalUpdateTimer ->setInterval(5000);
    connect(m_DglobalUpdateTimer, &QTimer::timeout, this, &mp_data::onGlobalTimerTimeout);

    connect(this,&mp_data::toJsonSig,mTojson,&mp_toJson::toJsonSlot);

}

void mp_data::udpsend() //发送数据
{
    std::unique_ptr<QUdpSocket> udpSocket = std::make_unique<QUdpSocket>();

    QString ipAddress = ui->serIp->text();
    QString portStr = ui->port->text();
    quint16 Port = portStr.toUShort();
    int num = ui->sendNum->value();
    QJsonObject json = mTojson->getJsonObject();
    QByteArray jsonData = QJsonDocument(json).toJson(QJsonDocument::Compact);
    udpSocket->writeDatagram(jsonData, QHostAddress(ipAddress), Port);

    int addr = json["addr"].toInt();
    for(int i = 1; i < num; i++){
        if(addr + i >=10 )break;
        QJsonObject modified = json;
        modified["addr"] = addr + i;
        QString dev_key = modified["dev_ip"].toString() + "-" + QString("%1").arg(addr+i, 1, 10, QChar('0'));
       // qDebug()<<dev_key<<' '<<modified["dev_ip"].toString()<<' '<<addr+i;
        modified["dev_key"] = dev_key;
        jsonData = QJsonDocument(modified).toJson(QJsonDocument::Compact);
        udpSocket->writeDatagram(jsonData, QHostAddress(ipAddress), Port);

        QThread::msleep(1);
    }
}

void mp_data::seriesinti()
{
    Group_series->addButton(ui->Aseries,0);
    Group_series->addButton(ui->Bseries,1);
    Group_series->addButton(ui->Cseries,2);
    Group_series->addButton(ui->Dseries,3);
    ui->Dseries->setChecked(1);
    connect(Group_series, &QButtonGroup::idClicked, this, &mp_data::seriesSelectionChanged);

    connect(this, &mp_data::seriesSelectionChanged, this, [=](int id) {
        qDebug() << "当前选择的系列ID:" << id;

        // 这里可以根据id做不同的处理
        switch(id) {
        case 0: Adatainti();  break;
        case 1: /* B系列处理 */ break;
        case 2: /* C系列处理 */ break;
        case 3: Ddatainti(); break;
        }
    });
    emit seriesSelectionChanged(3);
}

void mp_data::inti()
{
    ui->Table->setStyleSheet("QTabWidget:pane {border-top:0px solid #e8f3f9;background:  transparent; }");
    set_background_icon(this,":/image/box_back.jpg");

    Group_phase = new QButtonGroup(this);
    Group_series = new QButtonGroup(this);
    Group_phase->addButton(ui->spe,0);
    Group_phase->addButton(ui->tpe,1);
    ui->tpe->setChecked(1);

    temBoxes = { ui->envTem1, ui->envTem2,ui->envTem3, ui->envTem4 };
    humBoxes = { ui->envHum1, ui->envHum2,ui->envHum3, ui->envHum4 };

    sensors = {ui->sensor1,ui->sensor2,ui->sensor3,ui->sensor4};
    for(QCheckBox* sensor : sensors) {
        connect(sensor, &QCheckBox::stateChanged, this, &mp_data::sensorchange);
    }

    connect(ui->opbitNum, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &mp_data::bitCntChanged);  //改变输出位数量

    connect(ui->circuitNum, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &mp_data::criCntChanged); //改变回路数量
    connect(Group_phase, QOverload<int>::of(&QButtonGroup::idClicked),
            this, &mp_data::phaseData);

    connect(this,&mp_data::cuicurDatachange,this,&mp_data::cuicurData); //改变发送系列

    for(int i=0;i<4;i++){ //随机初始化温度湿度值

        temBoxes[i]->setValue(specRanNumGgen::get_temperature());
        humBoxes[i]->setValue(specRanNumGgen::get_humidity());
    }

    setBitcur();
}

void mp_data::Ddatainti() //数据初始化
{

    connect(ui->opbitTable, &QTableWidget::cellDoubleClicked,
            this, &mp_data::BitRowEdit);

    emit cuicurDatachange(4); //更新回路数据
}

void mp_data::Adatainti()
{
    emit cuicurDatachange(1);
}

void mp_data::DcuicurData()
{
    QTableWidget* bitTable = ui->opbitTable;    // 输出位表格
    QTableWidget* curTable = ui->circuitTable;  // 回路表格

    // 2. 初始化回路数据容器
    int totalCircuits = curTable->rowCount();
    QVector<double> circuitCurrents(totalCircuits, 0.000);
    QVector<double> circuitPower(totalCircuits, 0.000);      // 有功功率(行3)
    QVector<double> circuitReacPow(totalCircuits, 0.000);    // 无功功率(行4)
    QVector<double> circuitAppPow(totalCircuits, 0.000);     // 视在功率(行5)
    QVector<double> circuitEle(totalCircuits, 0.0);        // 电能(行7)

    // 3. 计算分配方案（最后一个回路最少）
    int totalOutputBits = bitTable->rowCount();
    int baseBits = totalOutputBits / totalCircuits;
    int remainder = totalOutputBits % totalCircuits;

    // 4. 数据累加核心逻辑（完全统一处理方式）
    int bitIndex = 0;
    for(int circuitIdx = 0; circuitIdx < totalCircuits; ++circuitIdx)
    {
        QTableWidgetItem* item = curTable->item(circuitIdx, 1);


        item = new QTableWidgetItem();
        curTable->setItem(circuitIdx, 1, item);

        // 必须设置单元格标志位
        item->setFlags(item->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        int voltage = ui->phaseV->value();

        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable); // 禁止直接编辑
        item->setData(Qt::EditRole, voltage);


        // 当前回路分配的输出位数
        int bitsThisCircuit = baseBits +
                              ((circuitIdx < remainder && circuitIdx != totalCircuits-1) ? 1 : 0);




        // 累加当前回路的所有输出位数据
        for(int i = 0; i < bitsThisCircuit; ++i)
        {


            bool x = (curTable->item(circuitIdx, 0)->checkState() == Qt::Checked) ? 1 : 0;
        // qDebug()<<circuitIdx<<' '<<bitIndex <<" "<<x;
            if (x == 0) {

                    bitTable->item(bitIndex, 0)->setText("断开");

            }

            if(bitIndex >= totalOutputBits) break;

            // 统一从输出位表格获取原始数据
            auto getData = [&](int col) -> double {
                if(QTableWidgetItem* item = bitTable->item(bitIndex, col)) {
                    bool ok;
                    double val = item->text().toDouble(&ok);
                    return ok ? val : 0.000;
                }
                return 0.0;
            };

            // 电流（列1）和其他功率参数同步累加
            if(x == 1 &&bitTable->item(bitIndex, 0)->text() == "闭合"){

                circuitCurrents[circuitIdx] += getData(1);  // 电流
                circuitPower[circuitIdx] += getData(2);      // 有功功率
                circuitReacPow[circuitIdx] += getData(3);   // 无功功率
                circuitAppPow[circuitIdx] += getData(4);
            }
                // 视在功率
            circuitEle[circuitIdx] += getData(6);       // 电能

            bitIndex++;

        }
    }




    // 5. 更新回路表格（强制创建项+统一格式化）
    auto updateTable = [&](int row, const QVector<double>& data) {
        for(int i = 0; i < totalCircuits; ++i) {
            QTableWidgetItem* item = curTable->item(i, row);
            if(!item) {
                item = new QTableWidgetItem();
                curTable->setItem(i, row, item);
            }
            item->setText(QString::number(data[i], 'f', 3));
        }
    };

    updateTable(2, circuitCurrents);  // 电流（行1）
    updateTable(3, circuitPower);     // 有功（行3）
    updateTable(4, circuitReacPow);   // 无功（行4）
    updateTable(5, circuitAppPow);    // 视在（行5）
    updateTable(7, circuitEle);       // 电能（行7）

    for(int circuitIdx = 0; circuitIdx < totalCircuits; ++circuitIdx){
        double pf = (circuitAppPow[circuitIdx] != 0)
        ? circuitPower[circuitIdx] / circuitAppPow[circuitIdx]
        : 0.0;
        pf = qRound(pf*100)/100.0;
        QTableWidgetItem* newItem = curTable->item(circuitIdx, 6);
        newItem = new QTableWidgetItem();
        curTable->setItem(circuitIdx, 6, newItem);
        newItem->setData(Qt::EditRole, pf);
        newItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable); // 禁止直接编辑

    }

}

void mp_data::cuicurData(int x)
{

        ui->Table->setTabEnabled(0, true);
        ui->Table->setCurrentIndex(0);
        DcuicurData();
        phaseData();

}

void mp_data::phaseData()
{
    QTableWidget* curTable = ui->circuitTable;
    int totalCircuits = curTable->rowCount();
    double CurrentA = 0,CurrentB = 0, CurrentC = 0;
    double powerA = 0 ,powerB = 0,powerC = 0;
    double rePowerA = 0 ,rePowerB = 0,rePowerC = 0;
    double AppPowerA = 0 ,AppPowerB = 0,AppPowerC = 0;
    double eleA =0 , eleB = 0, eleC =0;

    if(Group_phase->checkedId() == 0){ //单相
        for(int circuitIdx = 0; circuitIdx < totalCircuits; ++circuitIdx)
        {

            auto isCircuitOn = [&](int row) -> bool {
                if(QTableWidgetItem* item = curTable->item(row, 0)) { // 第0列是开关状态
                    return (item->text() == "1" || item->checkState() == Qt::Checked);
                }
                return false; // 默认关闭
            };
            double ele = curTable->item(circuitIdx,7)->text().toDouble();
            if(!isCircuitOn(circuitIdx)) {
                continue;
            }
            double current = curTable->item(circuitIdx, 2)->text().toDouble();
            double power = curTable->item(circuitIdx, 3)->text().toDouble();
            double rePower = curTable->item(circuitIdx, 4)->text().toDouble();
            double AppPower = curTable->item(circuitIdx, 5)->text().toDouble();
                CurrentA += current;
                powerA +=  power;
                rePowerA +=rePower;
                AppPowerA += AppPower;
                eleA += ele;
        }
        ui->cur_unbal_4->setVisible(0);
        ui->vol_unbal_4->setVisible(0);
        ui->volUnbal->setVisible(0);
        ui->curUnbal->setVisible(0);
    }
    else{//三相
        ui->cur_unbal_4->setVisible(1);
        ui->vol_unbal_4->setVisible(1);
        ui->volUnbal->setVisible(1);
        ui->curUnbal->setVisible(1);
        int baseBits = totalCircuits / 3;  // 三相均分
        int remainder = totalCircuits % 3;

        for(int circuitIdx = 0; circuitIdx < totalCircuits; ++circuitIdx)
        {
            // 确定当前回路所属相别

            auto isCircuitOn = [&](int row) -> bool {
                if(QTableWidgetItem* item = curTable->item(row, 0)) { // 第0列是开关状态
                    return (item->text() == "1" || item->checkState() == Qt::Checked);
                }
                return false; // 默认关闭
            };
            QString phase;
            if(circuitIdx < baseBits + (remainder > 0 ? 1 : 0)) {
                phase = "A";  // A相分配 baseBits + (remainder>0?1:0) 个回路
            }
            else if(circuitIdx < 2*baseBits + (remainder > 0 ? 1 : 0) + (remainder > 1 ? 1 : 0)) {
                phase = "B";  // B相分配 baseBits + (remainder>1?1:0) 个回路
            }
            else {
                phase = "C";  // C相分配剩余的回路
            }
           // qDebug() << "回路" << circuitIdx << "→" << phase;

            double current = curTable->item(circuitIdx, 2)->text().toDouble();
            double power = curTable->item(circuitIdx, 3)->text().toDouble();
            double rePower = curTable->item(circuitIdx, 4)->text().toDouble();
            double AppPower = curTable->item(circuitIdx, 5)->text().toDouble();
            double ele = curTable->item(circuitIdx,7)->text().toDouble();
            if(phase == "A") {
                if(isCircuitOn(circuitIdx)) {
                    CurrentA += current;
                    powerA +=  power;
                    rePowerA +=rePower;
                    AppPowerA += AppPower;
                }
                eleA += ele;
            }
            else if(phase == "B"){
                if(isCircuitOn(circuitIdx)) {
                    CurrentB += current;
                    powerB +=  power;
                    rePowerB += rePower;
                    AppPowerB += AppPower;
                }
                eleB += ele;
            }

            else if(phase == "C"){
                if(isCircuitOn(circuitIdx)) {
                    CurrentC += current;
                    powerC +=  power;
                    rePowerC += rePower;
                    AppPowerC += AppPower;
                }
                eleC += ele;
            }
        }
    }
    ui->volA->setValue(ui->phaseV->value()); ui->volB->setValue(ui->phaseV->value());ui->volC->setValue(ui->phaseV->value());

    ui->curA->setValue(CurrentA); ui->curB->setValue(CurrentB);ui->curC->setValue(CurrentC);
    if(CurrentA>=ui->phaseCur->value())ui->curA->setStyleSheet("color: red;"); else ui->curA->setStyleSheet("font-family: 微软雅黑;");
    if(CurrentB>=ui->phaseCur->value())ui->curB->setStyleSheet("color: red;"); else ui->curB->setStyleSheet("font-family: 微软雅黑;");
    if(CurrentC>=ui->phaseCur->value())ui->curC->setStyleSheet("color: red;"); else ui->curC->setStyleSheet("font-family: 微软雅黑;");

    ui->act_PowA->setValue(powerA);ui->act_PowB->setValue(powerB);ui->act_PowC->setValue(powerC);

    ui->Rec_PowA->setValue(rePowerA);ui->Rec_PowB->setValue(rePowerB);ui->Rec_PowC->setValue(rePowerC);

    ui->ApparentA->setValue(AppPowerA);ui->ApparentB->setValue(AppPowerB);ui->ApparentC->setValue(AppPowerC);

    ui->eleA->setValue(eleA);ui->eleB->setValue(eleB);ui->eleC->setValue(eleC);

    ui->pfA->setValue((AppPowerA != 0 ? powerA/AppPowerA:0.0));ui->pfB->setValue(AppPowerB != 0 ?powerB/AppPowerB:0.0);ui->pfC->setValue(AppPowerC != 0 ?powerC/AppPowerC:0.0);

    ui->powAct->setValue(powerA+powerB+powerC);

    ui->powReact->setValue(rePowerA+rePowerB+rePowerC);

    ui->totalEleact->setValue(eleA+eleB+eleC);

    ui->totalPA->setValue(AppPowerA+AppPowerB+AppPowerC);


    ui->totalPf->setValue((AppPowerA+AppPowerB+AppPowerC) == 0?0:(powerA+powerB+powerC)/(AppPowerA+AppPowerB+AppPowerC));

    if(Group_phase->checkedId() == 1){

        double volunbal = data_cal::calculateUnbalance(ui->volA->value(),ui->volB->value(),ui->volC->value());
        ui->volUnbal->setValue(volunbal);

        double curunbal = data_cal::calculateUnbalance(ui->curA->value(),ui->curB->value(),ui->curC->value());
        ui->curUnbal->setValue(curunbal);
    }
}

void mp_data::onGlobalTimerTimeout()
{
    int rowCount = ui->opbitTable->rowCount();

    for(int row = 0; row < rowCount; ++row) {

        QTableWidgetItem* switchItem = ui->opbitTable->item(row, 0);
        if(switchItem && switchItem->text() == "断开") {
            continue; // 开关关闭，跳过该行
        }

        //更新有功电能
        QTableWidgetItem* eleItem = ui->opbitTable->item(row, 6);
        QTableWidgetItem* powerItem = ui->opbitTable->item(row, 2);
        QVariant data1 = powerItem->data(Qt::DisplayRole);
        double value1 = data1.toDouble();
        QVariant data = eleItem->data(Qt::DisplayRole);
        double value = data.toDouble();
        eleItem->setData(Qt::EditRole,value+(value1*ui->timeInv->value()/3600));
        eleItem->setFlags(switchItem->flags() & ~Qt::ItemIsEditable);


        // 检查该行是否有电流增量设置（第7列）
        QTableWidgetItem* incrementItem = ui->opbitTable->item(row, 7);
        if(!incrementItem) continue;

        // 获取增量值（如果转换失败则默认为0）
        bool ok;
        int increment = incrementItem->text().toDouble(&ok);
        if(increment == 0)continue;
        double c = specRanNumGgen::getrandominc(increment);


        // 获取当前电流值（第1列）
        QTableWidgetItem* currentItem = ui->opbitTable->item(row, 1);
        if(!currentItem) continue;

        // 计算新电流值（限制在0-10范围内）
        double current = currentItem->text().toDouble(&ok);
        if(!ok) current = 0;

        double newCurrent = qBound(0.0, current + c, 10.0);

        // 更新电流值（保留2位小数）
        currentItem->setText(QString::number(newCurrent, 'f', 2));
        setBitPower(row);

    }
    DcuicurData();
    phaseData();

    timesend();
    udpsend();

}

void mp_data::setBitEle(const int row)
{

    QTableWidget* table = ui->opbitTable;
    QTableWidgetItem* ele = new QTableWidgetItem();

    ele->setData(Qt::EditRole,bitEle[row]);
    ele->setFlags(ele->flags() & ~Qt::ItemIsEditable);
    table->setItem(row,6,ele);
    ele->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter); // 右对齐
}

void mp_data::setBitPower(const int row)
{
    QTableWidget* table = ui->opbitTable;

    QTableWidgetItem* pf = new QTableWidgetItem(); //有功功率
    QTableWidgetItem* rePf = new QTableWidgetItem();//无功功率
    QTableWidgetItem* aPf = new QTableWidgetItem(); //视在功率

    double V = ui->phaseV->value();
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

void mp_data::setBitcur() //输出位电流初始化
{
    QTableWidget* table = ui->opbitTable;
    const int CURRENT_COL = 1; // 电流列索引
    // 设置电流列特性
    table->horizontalHeaderItem(CURRENT_COL)->setText("电流(A)"); // 列标题带单位
    table->setColumnWidth(CURRENT_COL, 100); // 固定列宽

    // 初始化所有行
    for(int row = 0; row < table->rowCount(); ++row) {
        QTableWidgetItem* currentItem = new QTableWidgetItem();
        currentItem->setData(Qt::EditRole, specRanNumGgen::getrandom(ui->bitCur->value())*100);
        currentItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter); // 右对齐
        currentItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable); // 禁止直接编辑
        table->setItem(row, CURRENT_COL, currentItem);
        // 创建表格项并设置文本
        QTableWidgetItem* item = new QTableWidgetItem(1);
        item->setText("闭合");
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);  // 移除可编辑标志
        item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter); // 右对齐->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter); // 右对齐
        table->setItem(row, 0, item);

        QTableWidgetItem* item1 = new QTableWidgetItem(1);
        item1->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter); // 右对齐
        item1->setData(Qt::EditRole,0);
        table->setItem(row, 7, item1);

        ranPf(row);
        setBitPower(row);
        setBitEle(row);
    }

    for(int i=0;i<ui->circuitTable->rowCount();i++){
        QTableWidgetItem* cbItem = new QTableWidgetItem("开关");
        cbItem->setCheckState(Qt::Checked);  // 默认勾选（原Qt::Unchecked改为Qt::Checked）
        cbItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        ui->circuitTable->setItem(i, 0, cbItem);
    }
}

void mp_data::BitRowEdit(int row)   //双击编辑行数据
{
    // 安全检查
    if(row < 0 || row >= ui->opbitTable->rowCount()) return;

    // 创建对话框
    QDialog dialog(this);
    dialog.setFixedSize(300, 200);
    dialog.setWindowTitle(QString("编辑输出位参数 - 第%1行").arg(row+1));
    QFormLayout layout(&dialog);

    // 定义要显示的参数列（索引、名称、验证规则）
    const QVector<QPair<int, QString>> params = {
        {0, "开关状态"},  // 第0列
        {1, "电流(A)"},   // 第1列
        {5, "功率因数"},  // 第5列
        {7, "电流增量(A)"} // 第7列
    };

    // 为每个参数创建编辑控件
    QVector<QWidget*> editors;
    QCheckBox* switchCheckBox = nullptr;

    for(const auto& param : params) {
        int col = param.first;

        if(col == 0) { // 开关状态列
            switchCheckBox = new QCheckBox(&dialog);

            // 获取单元格文本
            QString statusText = ui->opbitTable->item(row, col) ?
                                     ui->opbitTable->item(row, col)->text() : "";

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
                spinBox->setRange(0, 10);
                defaultValue = 1.0;
                spinBox->setSuffix(" A");
            }
            else if(col == 5) { // 功率因数列
                spinBox->setRange(0.2, 1);
                defaultValue = 0.3;

            }
            else if(col == 7) { // 电流增量列
                spinBox->setRange(0, 5);
                defaultValue = 0;
                spinBox->setSuffix(" A");
            }

            if(QTableWidgetItem* item = ui->opbitTable->item(row, col)) {
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
            QTableWidgetItem* item = ui->opbitTable->item(row, 0);
            if(!item) {
                item = new QTableWidgetItem();
                ui->opbitTable->setItem(row, 0, item);
            }
            item->setText(switchCheckBox->isChecked() ? "闭合" : "断开");
        }

        // 更新其他参数
        for(int i = 0; i < editors.size(); ++i) {
            int col = params[i+1].first; // +1因为跳过了开关状态
            QDoubleSpinBox* spinBox = qobject_cast<QDoubleSpinBox*>(editors[i]);

            QTableWidgetItem* item = ui->opbitTable->item(row, col);
            if(!item) {
                item = new QTableWidgetItem();
                ui->opbitTable->setItem(row, col, item);
            }
            item->setText(QString::number(spinBox->value(), 'f', 2));
        }

        // 触发表格更新信号
        emit ui->opbitTable->itemChanged(ui->opbitTable->item(row, 0));
        setBitPower(row);
    }
}

void mp_data::bitCntChanged()  //输出位数量改变
{
    int targetRows = ui->opbitNum->value();
    QTableWidget* table = ui->opbitTable;

    // 性能优化
    table->setUpdatesEnabled(false);
    QElapsedTimer timer;
    timer.start();

    // 记录当前选中行（保持操作焦点）
    int currentSelectedRow = table->currentRow();
    const int CURRENT_COL = 1;
    // 核心逻辑
    int currentRows = table->rowCount();
    table->setRowCount(targetRows);

    for (int i = 0; i < currentRows; i++) {
        QTableWidgetItem *item = table->item(i, 6);
        if (item) {
            bitEle[i] = std::max(item->text().toDouble(),bitEle[i]);
        }
    }


    // 初始化新增行
    for(int r = currentRows; r < targetRows; ++r) {
        if(!table->item(r, CURRENT_COL)) {
            QTableWidgetItem* currentItem = new QTableWidgetItem();
            currentItem->setData(Qt::EditRole, specRanNumGgen::getrandom(ui->bitCur->value())*100);
            currentItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            currentItem->setData(Qt::UserRole, QVariant(0.0)); // 存储原始数值
            table->setItem(r, CURRENT_COL, currentItem);
        }

        // 第一列：开关（带复选框）
        if(!table->item(r, 0)) {
            QTableWidgetItem* switchItem = new QTableWidgetItem(1);
            switchItem->setText("闭合");
            switchItem->setFlags(switchItem->flags() & ~Qt::ItemIsEditable);  // 移除可编辑标志
            switchItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            table->setItem(r, 0, switchItem);
        }

        // 其他列常规初始化
        for(int c = 1; c < table->columnCount(); ++c) {

            if(!table->item(r, c)) {
                table->setItem(r, c, new QTableWidgetItem());

                if(c == 6){
                    qDebug()<<table->item(r,6)->text().toDouble();
                    qDebug()<<bitEle[0];
                    setBitEle(r);
                }
                if(c == 5){
                    ranPf(r);
                    setBitPower(r);
                }
                if(c == 7){
                    QTableWidgetItem* item = new QTableWidgetItem(1);
                    item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter); // 右对齐
                    item->setData(Qt::EditRole,0);
                    table->setItem(r,7,item);

                }

            }
        }


    }

    // 恢复UI状态
    table->setCurrentCell(qMin(currentSelectedRow, targetRows-1), 0);
    table->setUpdatesEnabled(true);

}

void mp_data::criCntChanged() //回路电流个数变化
{
    // 获取目标回路数（电力系统专用命名）
    int circuitCount = ui->circuitNum->value();
    QTableWidget* table = ui->circuitTable;

    // 性能优化：禁用UI刷新
    table->setUpdatesEnabled(false);

    // 记录当前选中行（保持操作焦点）
    int currentSelectedRow = table->currentRow();

    // 核心行数调整
    int currentRows = table->rowCount();
    table->setRowCount(circuitCount);

    // 初始化新增的电力参数行
    for(int row = currentRows; row < circuitCount; ++row) {
        // 第一列：断路器状态（带复选框）
        QTableWidgetItem* cbItem = new QTableWidgetItem("开关");
        cbItem->setCheckState(Qt::Checked);  // 默认勾选（原Qt::Unchecked改为Qt::Checked）
        cbItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        table->setItem(row, 0, cbItem);

    }

    // 恢复UI状态
    table->setCurrentCell(qMin(currentSelectedRow, circuitCount-1), 0);
    table->setUpdatesEnabled(true);
}

void mp_data::sensorchange() //传感器状态
{
    for(int i=0;i<4;i++) {
        if(!sensors[i]->isChecked()) {  // 检查是否未选中
            humBoxes[i]->setEnabled(0);
            temBoxes[i]->setEnabled(0);
        }
        else {
            humBoxes[i]->setEnabled(1);
            temBoxes[i]->setEnabled(1);
        }
    }
}

mp_data::~mp_data()
{
    delete ui;
}

void mp_data::ranPf(const int row) //功率因素随机取值
{
    QTableWidget* table = ui->opbitTable;
    const int POWER_FAT = 5;

    QTableWidgetItem* pf = new QTableWidgetItem();
    pf->setData(Qt::EditRole, specRanNumGgen::get_power_factor_precise());
    pf->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter); // 右对齐
    pf->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable); // 禁止直接编辑
    table->setItem(row, POWER_FAT, pf);
}

void mp_data::on_sendJsonBtn_clicked()
{
    if(ui->sendJsonBtn->text() == "开始发送"){
        ui->sendJsonBtn->setText("停止发送");
        m_DglobalUpdateTimer->setInterval(ui->timeInv->value()*1000);
        m_DglobalUpdateTimer->start();
    }
    else{
        ui->sendJsonBtn->setText("开始发送");
        m_DglobalUpdateTimer->stop();
    }

}

void mp_data::timesend()
{
    // 1. 保存设备基本信息
    m_systemData.addr = ui->addr->value();

    int st = 0;
    m_systemData.devIp = ui->devIp->text();
    m_systemData.datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    // 2. 保存环境数据
    for(int i = 0; i < 4; i++) {
        m_systemData.pduData.envData.sensorStatuses[i] = sensors[i]->isChecked();
        if(sensors[i]->isChecked()) {
            m_systemData.pduData.envData.temperatures[i] = temBoxes[i]->value();
            m_systemData.pduData.envData.humidities[i] = humBoxes[i]->value();
        }
        else {
            m_systemData.pduData.envData.temperatures[i] = 0;
            m_systemData.pduData.envData.humidities[i] = 0;

        }
        // 计算露点温度
        m_systemData.pduData.envData.dewPoints[i] =
            data_cal::calculate_dewpoint1(temBoxes[i]->value(), humBoxes[i]->value());

        m_systemData.pduData.envData.hunAlarm[i] = 0;
        m_systemData.pduData.envData.temAlarm[i] = 0;
    }

    // 3. 保存输出位数据
    QTableWidget* bitTable = ui->opbitTable;
    int outputBitCount = bitTable->rowCount();
    m_systemData.pduData.outputData.outputBits.resize(outputBitCount);

    for(int row = 0; row < outputBitCount; row++) {
        OutputBit& bit = m_systemData.pduData.outputData.outputBits[row];
        bit.curAlarmMax = ui->bitCur->value();
        bit.current = bitTable->item(row, 1)->text().toDouble();

        if(bit.current>bit.curAlarmMax){bit.curAlarmStatus = 8;st = 2;}    //电流最大值告警
        else bit.curAlarmStatus = 0;

        bit.powAlarmStatus = 0;

        bit.switchStatus = (bitTable->item(row, 0)->text() == "闭合");
        bit.activePower = bitTable->item(row, 2)->text().toDouble();
        bit.reactivePower = bitTable->item(row, 3)->text().toDouble();
        bit.apparentPower = bitTable->item(row, 4)->text().toDouble();
        bit.powerFactor = bitTable->item(row, 5)->text().toDouble();
        bit.energy = bitTable->item(row, 6)->text().toDouble();

        bit.currentIncrement = bitTable->item(row, 7)->text().toDouble();
    }

    // 4. 保存回路数据
    QTableWidget* circuitTable = ui->circuitTable;
    int circuitCount = circuitTable->rowCount();

    // 初始化回路数据容器
    m_systemData.pduData.loopData.Circuits.resize(outputBitCount);


    for(int row = 0; row < circuitCount; row++) {
        Circuit& cir = m_systemData.pduData.loopData.Circuits[row];

        cir.curAlarmMax = ui->circuitCur->value();
        cir.voltage = circuitTable->item(row,1)->text().toDouble();
        cir.current = circuitTable->item(row,2)->text().toDouble();
        cir.breakerStatus = (circuitTable->item(row, 0)->checkState() == Qt::Checked) ? 1 : 0;
        cir.activePower = circuitTable->item(row,3)->text().toDouble();
        cir.reactivePower = circuitTable->item(row,4)->text().toDouble();
        cir.apparentPower = circuitTable->item(row,5)->text().toDouble();
        cir.powerFactor = circuitTable->item(row,6)->text().toDouble();
        cir.energy = circuitTable->item(row,7)->text().toDouble();

        cir.curAlarmStatus = (cir.current>cir.curAlarmMax ? 8 : 0);
        if(cir.curAlarmStatus) st = 2;
        cir.powAlarmStatus = 0;
        cir.volAlarmStatus = 0;
    }

    // 5. 保存相位数据
    bool isSinglePhase = (Group_phase->checkedId() == 0);
    m_systemData.pduData.phases.initialize(isSinglePhase);

    m_systemData.pduData.phases.voltages[0] = ui->volA->value();
    m_systemData.pduData.phases.currents[0] = ui->curA->value();
    m_systemData.pduData.phases.activePowers[0] = ui->act_PowA->value();
    m_systemData.pduData.phases.reactivePowers[0] = ui->Rec_PowA->value();
    m_systemData.pduData.phases.apparentPowers[0] = ui->ApparentA->value();
    m_systemData.pduData.phases.energies[0] = ui->eleA->value();
    m_systemData.pduData.phases.powerFactors[0] = ui->pfA->value();

    if(!isSinglePhase) {


        m_systemData.pduData.phases.voltages[1] = ui->volB->value();
        m_systemData.pduData.phases.voltages[2] = ui->volC->value();

        m_systemData.pduData.phases.currents[1] = ui->curB->value();
        m_systemData.pduData.phases.currents[2] = ui->curC->value();

        m_systemData.pduData.phases.activePowers[1] = ui->act_PowB->value();
        m_systemData.pduData.phases.activePowers[2] = ui->act_PowC->value();

        m_systemData.pduData.phases.reactivePowers[1] = ui->Rec_PowB->value();
        m_systemData.pduData.phases.apparentPowers[1] = ui->ApparentB->value();
        m_systemData.pduData.phases.energies[1] = ui->eleB->value();
        m_systemData.pduData.phases.powerFactors[1] = ui->pfB->value();

        m_systemData.pduData.phases.reactivePowers[2] = ui->Rec_PowC->value();
        m_systemData.pduData.phases.apparentPowers[2] = ui->ApparentC->value();
        m_systemData.pduData.phases.energies[2] = ui->eleC->value();
        m_systemData.pduData.phases.powerFactors[2] = ui->pfC->value();
    }

    // 6. 保存全局统计数据
    m_systemData.pduData.totalData.powActive = ui->powAct->value();
    m_systemData.pduData.totalData.powReactive = ui->powReact->value();
    m_systemData.pduData.totalData.powApparent = ui->totalPA->value();
    m_systemData.pduData.totalData.powerFactor = ui->totalPf->value();
    m_systemData.pduData.totalData.eleActive = ui->totalEleact->value();
    m_systemData.pduData.totalData.volUnbalance = ui->volUnbal->value();
    m_systemData.pduData.totalData.curUnbalance = ui->curUnbal->value();

    // 7. 保存系统设置
    if(ui->Aseries->isChecked()) m_systemData.settings.series = "A";
    else if(ui->Bseries->isChecked()) m_systemData.settings.series = "B";
    else if(ui->Cseries->isChecked()) m_systemData.settings.series = "C";
    else m_systemData.settings.series = "D";

    m_systemData.settings.phaseMode = ui->spe->isChecked() ? "single" : "three";
    m_systemData.settings.outputBitCount = ui->opbitNum->value();
    m_systemData.settings.circuitCount = ui->circuitNum->value();

    // 8. 保存阈值设置
    m_systemData.thresholds.capVol = ui->phaseV->value();
    m_systemData.thresholds.capPhaseCur = ui->phaseCur->value();
    m_systemData.thresholds.capbitCur = ui->bitCur->value();
    m_systemData.thresholds.capCirCur = ui->bitCur->value();

    m_systemData.status = 0;

    emit toJsonSig(m_systemData);

}

void mp_data::saveSettings(QSettings &settings)
{
    bitCntChanged();
    settings.beginGroup("MpbitEle");
    for (int i = 0; i < 50; ++i) {
        settings.setValue(QString("bitEle_%1").arg(i), bitEle[i]);
    }
    settings.endGroup();
    settings.sync();
}

void mp_data::loadSettings(QSettings &settings)
{
    bitCntChanged();
    settings.beginGroup("MpbitEle");
    for (int i = 0; i < 50; ++i) {
        bitEle[i] = settings.value(QString("bitEle_%1").arg(i), 0.0).toDouble();
    }
    settings.endGroup();
}

void mp_data::hideEvent(QHideEvent *event)
{
    qDebug() << "mp_data hideEvent triggered";
    bitCntChanged();
    QString configPath = QCoreApplication::applicationDirPath() + "/config.ini";
    QSettings settings(configPath, QSettings::IniFormat);
    saveSettings(settings);

    QWidget::hideEvent(event);  // 调用基类实现
}
