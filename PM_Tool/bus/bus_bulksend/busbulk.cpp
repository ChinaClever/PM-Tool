#include "busbulk.h"
#include "ui_busbulk.h"
#include "bus_globals.h"
#include "specrannumggen.h"
#include "data_cal/data_cal.h"
#include "stylehelper.h"
#include "databasemanager.h"
#include <QDebug>
#include <math.h>
const int BusNum = 3;
busBulk::busBulk(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::busBulk)
    , mMapProcessor(new MapProcessor(this))
{
    ui->setupUi(this);
    for(int i = 0; i < BusNum; i++){
        mJsonQ.push_back(new MapJsonQueue(this));
    }

    init();
}

void busBulk::init()
{
    StyleHelper::setLightBlueButton(ui->busSendBtn);

    ui->devIp->setValidator(new QRegExpValidator(QRegExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b")));

    bus_sendTime = ui->bus_timeInv->value();
    connect(this,&busBulk::ProcessorRun,mMapProcessor,&MapProcessor::PRun);
    connect(mMapProcessor, &MapProcessor::checkSend,
            this, &busBulk::checkSend);
    for(int i = 0; i < BusNum; i++){
        connect(this,&busBulk::ProcessorRun,mJsonQ[i],&MapJsonQueue::JRun);
        connect(mJsonQ[i], &MapJsonQueue::checkTime,
                this, &busBulk::checkTime);
    }
}

void busBulk::updateButtonState(bool x)
{
    ui->boxNum->setEnabled(x);
    ui->busNum->setEnabled(x);
    ui->devIp->setEnabled(x);
    ui->volCap->setEnabled(x);
    ui->cirCurCap->setEnabled(x);
}

void busBulk::DataResize(Busbar& bus) //resize数组大小
{
    bus.init(3,4);
    bus.boxData.loopItemList.init(9);
    bus.boxData.outletItemList.init(3);
    bus.boxData.lineItemList.init(3);

}

void busBulk::envTem(EnvItem& u){
    for(int i = 0; i < 4; i ++){
        u.temValue[i] = specRanNumGgen::get_temperature();
        u.boxtem[i] = specRanNumGgen::get_temperature();
    }
}

void busBulk::setInfo(Info &info)
{
    info.barId = 1;
    info.status = 1;
    info.devIp = devip;
    info.datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    info.busKey = info.devIp+"-"+"1";
}

void busBulk::setBoxCfg(BoxConfig& cfg)
{
    cfg.alarmCount = 5;
    cfg.iof = 1;
    cfg.boxVersion = "1";
    cfg.baudRate = 9600;
    cfg.beep = 1;
    cfg.workMode = 2;
    cfg.itemType = 1;
    cfg.loopNum = 9;
    cfg.boxType = 0;
    cfg.breakerStatus.resize(cfg.loopNum);
    for(int i = 0; i < cfg.loopNum; i ++){
        cfg.breakerStatus[i] = 1;
    }
}

void busBulk::setBoxLoop(BoxData& box)
{
    auto &u = box.loopItemList;
    for(int i = 0; i < box.boxCfg.loopNum; i ++){
        u.volValue[i] = ui->volCap->value();
        u.volMax[i] = u.volValue[i];

        u.curValue[i] = specRanNumGgen::generate_phase_current(ui->cirCurCap->value());
        u.curMax[i] = ui->cirCurCap->value();

        u.powerFactor[i] = specRanNumGgen::get_power_factor_precise();

        u.powMax[i] = 100;


        u.powApparent[i] = u.volValue[i] * u.curValue[i]/1000.0;
        u.powValue[i] = u.volValue[i] * u.curValue[i] * u.powerFactor[i] / 1000.0;
        u.powReactive[i] = data_cal::reactive_powerCal(u.powApparent[i],u.powValue[i]);

        u.eleReactive[i] = 0;
        u.eleActive[i] = 0;
    }

}

void busBulk::setBoxList(BoxData& box)
{
    auto &u = box.lineItemList;
    auto &v = box.outletItemList;
    auto &x = box.loopItemList;

    for(int i = 0; i < 3; i++){

        u.curValue[i] = 0;
        u.powActive[i] = 0;
        u.powApparent[i] = 0;
        u.powReactive[i] = 0;
        u.eleActive[i] = 0;
        u.eleReactive[i] = 0;

        v.powActive[i] = 0;
        v.powApparent[i] = 0;
        v.powReactive[i] = 0;
        v.eleActive[i] = 0;
        v.eleReactive[i] = 0;

    }

    for(int i = 0; i < box.boxCfg.loopNum; i ++){
        int id = i%3;
        u.volValue[id] = x.volValue[i];
        u.curValue[id] += x.curValue[i];
        u.curThd[id] = specRanNumGgen::getCurThd();
        u.powActive[id] += x.powValue[i];
        u.powApparent[id] += x.powApparent[i];
        u.powReactive[id] += x.powReactive[i];
        u.eleActive[id] += x.eleActive[i];
        u.eleReactive[id] += x.eleReactive[i];

        id = i/2;
        v.powActive[id] += x.powValue[i];
        v.powApparent[id] += x.powApparent[i];
        v.powReactive[id] += x.powReactive[i];
        v.eleActive[id] += x.eleActive[i];
        v.eleReactive[id] += x.eleReactive[i];
    }

    for(int i = 0; i < 3; i++){
        bulkPhase[0][i] += u.curValue[i]; //电流
        bulkPhase[1][i] += u.powActive[i]; //有功功率
        bulkPhase[2][i] += u.powApparent[i]; //视在功率
        bulkPhase[3][i] += u.powReactive[i]; //无功功率
        bulkPhase[4][i] += u.eleActive[i]; //电能
        bulkPhase[5][i] += u.eleReactive[i]; //无功电能
    }

    for(int i = 0; i < 3; i++){
        u.loadRate[i] = u.curValue[i] / x.curMax[i];
        u.powerFactor[i] = u.powActive[i] / u.powApparent[i];
        v.powerFactor[i] = v.powActive[i] / v.powApparent[i];
        v.eleApparent[i] = sqrt(v.eleActive[i] * v.eleActive[i] + v.eleReactive[i] * v.eleReactive[i]);

    }

    {
        box.boxTotalData.powActive = 0;
        box.boxTotalData.powApparent = 0;
        box.boxTotalData.powReactive = 0;
        box.boxTotalData.eleActive = 0;
        box.boxTotalData.eleReactive = 0;
    }

    for(int i = 0; i < 3; i++){
        box.boxTotalData.powActive += u.powActive[i];
        box.boxTotalData.powApparent += u.powApparent[i];
        box.boxTotalData.powReactive += u.powReactive[i];
        box.boxTotalData.eleActive += u.eleActive[i];
        box.boxTotalData.eleReactive += u.eleReactive[i];
    }

    box.boxTotalData.powerFactor = box.boxTotalData.powActive / box.boxTotalData.powApparent *1.0;
    box.boxTotalData.eleApparent = sqrt(box.boxTotalData.eleActive * box.boxTotalData.eleActive
                                        + box.boxTotalData.eleReactive * box.boxTotalData.eleReactive);
}

void busBulk::setBusCfg(BusConfig& cfg)
{
    cfg.vol = ui->volCap->value();
    cfg.curSpecs = 800;
    cfg.busVersion = 2;
    cfg.baudRate = 1;
    cfg.beep = 0;
    cfg.workMode = 0;
    cfg.acDc = 0;
    cfg.itemType = 0;
    cfg.boxNum = ui->boxNum->value();
    cfg.alarmCount = 5;
    cfg.iof = 1;
    cfg.isd = 1;
    cfg.shuntTrip = 1;
    cfg.breakerStatus = 1;
    cfg.lspStatus = 1;
}

void busBulk::setBusline(BusData& bus)
{
    auto& u = bus.lineItemList;
    for(int i = 0; i < 3; i ++){
        u.volValue[i] = bus.busCfg.vol;
        u.volMax[i] = u.volValue[i];
        u.volThd[i] = specRanNumGgen::getCurThd();

        u.curValue[i] = bulkPhase[0][i];
        u.curMax[i] = 800;
        u.curThd[i] = specRanNumGgen::getCurThd();

        u.powValue[i] = bulkPhase[1][i];
        u.powMax[i] = 100;
        u.powApparent[i] = bulkPhase[2][i];
        u.powReactive[i] = bulkPhase[3][i];

        u.eleActive[i] = bulkPhase[4][i];
        u.eleReactive[i] = bulkPhase[5][i];

        u.volLineValue[i] = sqrt(u.volValue[i]);
        u.volLineMax[i] = u.volLineValue[i];

        u.loadRate[i] = u.curValue[i] / u.curMax[i];
        u.powerFactor[i] = u.powValue[i] / u.powApparent[i];
    }
}

void busBulk::setBusTotal(BusData& bus)
{
    auto &u = bus.busTotalData;
    auto &v = bus.lineItemList;

    {
        u.powValue = 0;
        u.powApparent = 0;
        u.powReactive = 0;
        u.eleActive = 0;
        u.eleReactive = 0;
        u.curResidualValue = 800;
    }

    for(int i = 0; i < 3; i++){
        u.powValue += v.powValue[i];
        u.powApparent += v.powApparent[i];
        u.powReactive += v.powReactive[i];
        u.eleActive += v.eleActive[i];
        u.eleReactive += v.eleReactive[i];
        u.curResidualValue -= v.curValue[i];
    }
  //  qDebug()<<u.powValue <<' '<<u.powApparent;
    u.powerFactor = u.powValue / u.powApparent *1.0;
    u.hzValue = 55;
    u.hzMax = 100;
    u.powMax = 1200;
    if(u.powValue > u.powMax)u.powStatus = 2;

    u.volUnbalance = 0;

    u.curUnbalance = data_cal::calculateUnbalance(v.curValue[0],v.curValue[1],v.curValue[2]);
}

void busBulk::bulkinti(const int x)
{
    bulkPhase = QVector<QVector<double>>(6, QVector<double>(3, 0.0));
    auto Bus = Busbar();
    DataResize(Bus);   //回路数量修改
    envTem(Bus.envItemList);
    setInfo(Bus.info);

    for(int i = 0; i < ui->boxNum->value(); i++){
        Bus.info.addr = i + 2;
        Bus.flag = 1;
        auto Box = BoxData();
        setBoxCfg(Box.boxCfg);
        Box.boxCfg.workMode = i + 2;
        Box.loopItemList.init(9);   //回路数量
        Box.outletItemList.init(3);
        Box.lineItemList.init(3);


        setBoxLoop(Box);
        setBoxList(Box);

        Bus.boxData = Box;
        Bus.info.boxName = "Box-"+QString("%1").arg(i+1);
        Bus.info.busName = "Bus-1";
        Bus.info.boxKey = Bus.info.busKey+"-"+QString("%1").arg(i+2);

        {
            double eleActive[6] = {0};
            double eleReactive[6] = {0};
            QString key = Bus.info.boxKey;

            bool ok = DatabaseManager::instance().queryBoxPhaseEnergy(key, eleActive, eleReactive);
            if (ok) {
                for (int i = 0; i < 6; i++) {
                    Box.loopItemList.eleActive[i] = eleActive[i];
                    Box.loopItemList.eleReactive[i] = eleReactive[i];
                }
            } else {
                // 数据库无此key，赋默认值0
                for (int i = 0; i < 6; i++) {
                    Box.loopItemList.eleActive[i] = 0;
                    Box.loopItemList.eleReactive[i] = 0;
                }
                // 自动插入一条新记录，初始值全0
                DatabaseManager::instance().insertOrUpdateBoxPhaseEnergy(key, eleActive, eleReactive);
            }

        }

        busMap[index++] = Bus ;
    }

    setBusCfg(Bus.busData.busCfg);
    setBusline(Bus.busData);
    setBusTotal(Bus.busData);
    Bus.flag = 0;
    Bus.info.addr = 1;
    Bus.info.busName = "Bus-1";
    {
        //qDebug()<<Bus.info.busKey;
        //如果key存在，读写数据库文件
    }
    busMap[index++] = Bus ;
}

void busBulk::intiMap()
{
    devip = ui->devIp->text();

    for(int i = 0; i < ui->busNum->value(); i++){
        if(i!=0){
            devip = data_cal::getNextIp(devip);
        }
        bulkinti(i);
    }



}

void busBulk::on_busSendBtn_clicked()
{
    if(ui->busSendBtn->text() == "开始发送"){
        ui->busSendBtn->setText("停止发送");
        updateButtonState(0);
        bulkBoxNum = ui->boxNum->value();
        bulkBusNum = ui->busNum->value();
        index = 1;
        intiMap();
        processStart();


    }
    else{
        emit ProcessorRun(0);
        updateButtonState(1);
        ui->busSendBtn->setText("开始发送");
    }
}

void busBulk::triggerToggleSend(bool flag)
{
    if (flag) { // 开始发送
        if (ui->busSendBtn->text() == "开始发送") {
            ui->busSendBtn->setText("停止发送");
            updateButtonState(0);
            bulkBoxNum = ui->boxNum->value();
            index = 1;
            intiMap();
            processStart();
        }
    } else { // 停止发送
        if (ui->busSendBtn->text() == "停止发送") {
            emit ProcessorRun(0);
            updateButtonState(1);
            ui->busSendBtn->setText("开始发送");
        }
    }
}

void busBulk::processStart()
{
    emit ProcessorRun(1);
    mMapProcessor->start();
    for(int i = 0; i < BusNum; i ++){
        mJsonQ[i]->start();
    }
}

busBulk::~busBulk()
{
    {
        QMutexLocker locker(&busBulkJQMutexes);
        busQueue.clear();
    }

    // 断开与 mJsonQ 的信号槽连接（可保留）
    for (int i = 0; i < mJsonQ.size(); ++i) {
        if (mJsonQ[i]) {
            disconnect(mJsonQ[i], nullptr, this, nullptr);
            mJsonQ[i]->JRun(false);
            mJsonQ[i]->quit();
            mJsonQ[i]->wait();
            // 不要 delete 和置 nullptr
        }
    }

    if (mMapProcessor) {
        disconnect(mMapProcessor, nullptr, this, nullptr);
        emit ProcessorRun(0);
        mMapProcessor->PRun(false);
        mMapProcessor->quit();
        mMapProcessor->wait();
        // 不要 delete 和置 nullptr
    }

    // 不需要 mJsonQ.clear();
    delete ui;
}



void busBulk::on_bus_timeInv_valueChanged(int arg1)
{
    bus_sendTime = arg1;
}

void busBulk::checkSend(int a,int b,int c)
{
   // qDebug()<<a<<"  "<<b<<"  "<<c<<"!"<<endl;
    ui->ErNum->setValue(c);
    ui->successNum->setValue(b);
}

void busBulk::checkTime(int t)
{
    //qDebug()<<t;
    ui->successTime->setValue(t/1000.0);
}


void busBulk::saveSettings(QSettings &settings) {
    settings.beginGroup("Busbulk");
    SettingsHelper::saveSpinBox(settings, "busNum", ui->busNum);
    SettingsHelper::saveSpinBox(settings, "boxNum", ui->boxNum);
    settings.endGroup();
}

void busBulk::loadSettings(QSettings &settings) {
    settings.beginGroup("Busbulk");
    SettingsHelper::loadSpinBox(settings, "busNum", ui->busNum);
    SettingsHelper::loadSpinBox(settings, "boxNum", ui->boxNum);
    settings.endGroup();
}

