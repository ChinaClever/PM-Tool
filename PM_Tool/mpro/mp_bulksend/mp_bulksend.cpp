#include "mp_bulksend.h"
#include "ui_mp_bulksend.h"
#include "mp_globals.h"
#include "data_cal/data_cal.h"
#include "specrannumggen.h"
#include "stylehelper.h"
#include <QDebug>
#include <math.h>
const int ProNum = 5;

mp_bulksend::mp_bulksend(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::mp_bulksend)
    , mAMapProcessor(new AMapProcessor(this))
    , mBMapProcessor(new BMapProcessor(this))
    , mCMapProcessor(new CMapProcessor(this))
    , mDMapProcessor(new DMapProcessor(this))
{
    ui->setupUi(this);

    for(int i = 0; i < ProNum; i++){
        mAJsonQ.push_back(new AMapJsonQueue(this));
        mBJsonQ.push_back(new BMapJsonQueue(this));
        mCJsonQ.push_back(new CMapJsonQueue(this));
        mDJsonQ.push_back(new DMapJsonQueue(this));
    }
    inti();
}

void mp_bulksend::inti()
{
    mp_sendTime = ui->mp_timeInv->value();
    StyleHelper::setLightBlueButton(ui->mpSendBtn);
    serNum.resize(4); //abcd四个系列
    addr.resize(4);
    key.resize(4);
    devip.resize(4);

    connect(this,&mp_bulksend::ProcessorRun,mAMapProcessor,&AMapProcessor::APRun);
    connect(this,&mp_bulksend::ProcessorRun,mBMapProcessor,&BMapProcessor::BPRun);
    connect(this,&mp_bulksend::ProcessorRun,mCMapProcessor,&CMapProcessor::CPRun);
    connect(this,&mp_bulksend::ProcessorRun,mDMapProcessor,&DMapProcessor::DPRun);

    for(int i = 0; i < ProNum; i ++){
        connect(this,&mp_bulksend::ProcessorRun,mAJsonQ[i],&AMapJsonQueue::AJRun);
        connect(this,&mp_bulksend::ProcessorRun,mBJsonQ[i],&BMapJsonQueue::BJRun);
        connect(this,&mp_bulksend::ProcessorRun,mCJsonQ[i],&CMapJsonQueue::CJRun);
        connect(this,&mp_bulksend::ProcessorRun,mDJsonQ[i],&DMapJsonQueue::DJRun);

    }

    QLineEdit* devIpEdits[] = { ui->AdevIp, ui->BdevIp, ui->CdevIp, ui->DdevIp };
    QRegExp ipRegex("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");

    for (int i = 0; i < 4; ++i) {
        devIpEdits[i]->setValidator(new QRegExpValidator(ipRegex, this));  // 设置父对象 Qt 会自动释放
    }
    initdevip();

    QList<QSpinBox*> spinBoxes = { ui->mp_timeInv, ui->cirNum, ui->bitNum, ui->machNum, ui->volCap,
                                   ui->AsendNum, ui->BsendNum, ui->CsendNum, ui->DsendNum };

    intchange();
    for (QSpinBox* spinBox : spinBoxes) {
        connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                this, &mp_bulksend::intchange);
    }
}

void mp_bulksend::initdevip()
{
    QLineEdit* devIpEdits[] = { ui->AdevIp, ui->BdevIp, ui->CdevIp, ui->DdevIp };
    for (int i = 0; i < 4; ++i) {
        devip[i] = devIpEdits[i]->text();
        addr[i] = -1;
    }
}

void mp_bulksend::intchange()
{
    mp_sendTime = ui->mp_timeInv->value();
    cirNum = ui->cirNum->value();
    bitNum = ui->bitNum->value();
    macNum = ui->machNum->value();
    volCap = ui->volCap->value();
    serNum[0] = ui->AsendNum->value();
    serNum[1] = ui->BsendNum->value();
    serNum[2] = ui->CsendNum->value();
    serNum[3] = ui->DsendNum->value();
}

void mp_bulksend::on_mpSendBtn_clicked()
{
    if(ui->mpSendBtn->text() == "开始发送"){
        ui->mpSendBtn->setText("停止发送");
        initdevip();
        updateButtonState(0);

        intiMap();
        processStart();

    }
    else{
        emit ProcessorRun(0);
        updateButtonState(1);
        ui->mpSendBtn->setText("开始发送");
    }
}

void mp_bulksend::triggerToggleSend(bool flag)
{
    bool isSending = (ui->mpSendBtn->text() == "停止发送"); // 当前是否正在发送

    if (flag && !isSending) {
        // 传入要求开始发送，但当前没在发送，调用开始发送槽
        on_mpSendBtn_clicked();
    } else if (!flag && isSending) {
        // 传入要求停止发送，但当前正在发送，调用停止发送槽
        on_mpSendBtn_clicked();
    }
    // 如果状态已经符合要求，不调用槽函数
}

void mp_bulksend::processStart()
{
    emit ProcessorRun(1);
    mAMapProcessor->start();
    mBMapProcessor->start();
    mCMapProcessor->start();
    mDMapProcessor->start();
    for(int i = 0; i < ProNum; i++){
        mAJsonQ[i]->start();
        mBJsonQ[i]->start();
        mCJsonQ[i]->start();
        mDJsonQ[i]->start();
    }
}

void mp_bulksend::updateButtonState(bool flag)
{
    ui->machNum->setEnabled(flag);
    ui->cirNum->setEnabled(flag);
    ui->bitNum->setEnabled(flag);
    ui->volCap->setEnabled(flag);
}

mp_bulksend::~mp_bulksend()
{
    delete ui;
}

void mp_bulksend::intiMap()
{
    for(int i = 0;i < serNum[0];i ++){
        bulkinti(0);
    }
    for(int i = 0;i < serNum[1];i ++){
        bulkinti(1);
    }
    for(int i = 0;i < serNum[2];i ++){
        bulkinti(2);
    }
    for(int i = 0;i < serNum[3];i ++){
        bulkinti(3);
    }
}

void mp_bulksend::bulkinti(const int x)
{
    auto packet = PowerSystemData();
    dataResize(packet);

    QString key = data_cal::generateNextCascadeIP(devip[x],addr[x],macNum);

    packet.initializeInfo(devip[x],addr[x],key);

    setInti(packet);
    envInti(packet);    //环境初始化
    bitInti(packet);
    cirInti(packet);
    lineInti(packet);
    totalInti(packet);

    if(x == 0){
        packet.settings.series = "A";
        AMap[key] = packet;
    }
    else if(x == 1){
        packet.settings.series = "B";
        BMap[key] = packet;
    }
    else if(x == 2){
        packet.settings.series = "C";
        CMap[key] = packet;
    }
    else {
        packet.settings.series = "D";
        DMap[key] = packet;
    }
    //debugPrintSystemData(packet);

}

void mp_bulksend::setInti(PowerSystemData& u)
{
    u.settings.circuitCount = u.pduData.loopData.Circuits.size();
    u.settings.outputBitCount = u.pduData.outputData.outputBits.size();
    u.settings.phaseMode = "three";
    u.thresholds.capPhaseCur = 63;
    u.thresholds.capCirCur = 16;
    u.thresholds.capVol = volCap;
    u.thresholds.capbitCur = 10;
}

void mp_bulksend::totalInti(PowerSystemData& u)
{
    int size = u.pduData.phases.phaseNum;

    u.pduData.totalData.powActive = 0;
    u.pduData.totalData.powApparent = 0;
    u.pduData.totalData.powReactive = 0;
    u.pduData.totalData.eleActive = 0;
    u.pduData.totalData.curValue = 0;

    for(int i = 0; i <size; ++i){
        u.pduData.totalData.curValue += u.pduData.phases.currents[i];
        u.pduData.totalData.powActive += u.pduData.phases.activePowers[i];
        u.pduData.totalData.powApparent += u.pduData.phases.apparentPowers[i];
        u.pduData.totalData.powReactive += u.pduData.phases.reactivePowers[i];
        u.pduData.totalData.eleActive += u.pduData.phases.energies[i];
    }

    u.pduData.totalData.powerFactor = (u.pduData.totalData.powApparent != 0 ?
                                        u.pduData.totalData.powActive/u.pduData.totalData.powApparent : 0);

    double  cnt = (*std::max_element(u.pduData.phases.currents.begin(),u.pduData.phases.currents.end()))
        -(*std::min_element(u.pduData.phases.currents.begin(),u.pduData.phases.currents.end()));
    double avg = u.pduData.totalData.curValue/3.0;
    u.pduData.totalData.curUnbalance = avg != 0 ? cnt / avg : 0;
}

void mp_bulksend::lineInti(PowerSystemData& u)
{
    int size = u.pduData.phases.phaseNum;
    int bnum = u.pduData.loopData.Circuits.size();
    int base = bnum / size;  // 每个相基础分配数
    int remainder = bnum % size; // 剩余回路

    int cirIndex = 0; // 当前处理的回路索引
    for(int i = 0; i < size; ++i){

        u.pduData.phases.voltages[i] = u.pduData.loopData.Circuits[0].voltage;
        u.pduData.phases.volStatus[i] = 0;
        u.pduData.phases.curMax[i] = u.thresholds.capPhaseCur;

        {
            u.pduData.phases.currents[i] = 0;u.pduData.phases.activePowers[i] = 0;
            u.pduData.phases.apparentPowers[i] = 0;u.pduData.phases.reactivePowers[i] = 0;
            u.pduData.phases.energies[i] = 0;
        }

        int ThisLoop = base + (i < remainder ? 1 : 0);
        for(int j = 0; j < ThisLoop && cirIndex < bnum; ++j){
            u.pduData.phases.currents[i] += u.pduData.loopData.Circuits[cirIndex].current;
            u.pduData.phases.activePowers[i] += u.pduData.loopData.Circuits[cirIndex].activePower;
            u.pduData.phases.apparentPowers[i] += u.pduData.loopData.Circuits[cirIndex].apparentPower;
            u.pduData.phases.reactivePowers[i] += u.pduData.loopData.Circuits[cirIndex].reactivePower;
            u.pduData.phases.energies[i] += u.pduData.loopData.Circuits[cirIndex].energy;
            cirIndex++;
        }

        u.pduData.phases.powerFactors[i] = (u.pduData.phases.apparentPowers[i] != 0 ?
                                u.pduData.phases.activePowers[i]/u.pduData.phases.apparentPowers[i] : 0);
        u.pduData.phases.phase_voltage[i] = u.pduData.phases.voltages[i]/sqrt(3);

        if(u.pduData.phases.currents[i] > u.pduData.phases.curMax[i])u.pduData.phases.curStatus[i] = 8;
    }
}

void mp_bulksend::cirInti(PowerSystemData& u)
{
    int size = u.pduData.loopData.Circuits.size();
    int bnum = u.pduData.outputData.outputBits.size();
    int base = bnum / size;  // 每个回路基础分配数
    int remainder = bnum % size; // 剩余输出位

    int bitIndex = 0; // 当前处理的输出位索引
    for(int i = 0; i < size; ++i){
        u.pduData.loopData.Circuits[i].breakerStatus = 1;
        u.pduData.loopData.Circuits[i].curAlarmMax = u.thresholds.capCirCur;
        u.pduData.loopData.Circuits[i].voltage = u.pduData.outputData.outputBits[0].vol;
        u.pduData.loopData.Circuits[i].volAlarmStatus = 0;

        int bitsThisLoop = base + (i < remainder ? 1 : 0);

        {
            u.pduData.loopData.Circuits[i].current = 0;
            u.pduData.loopData.Circuits[i].activePower = 0;
            u.pduData.loopData.Circuits[i].apparentPower = 0;
            u.pduData.loopData.Circuits[i].reactivePower = 0;
            u.pduData.loopData.Circuits[i].energy = 0;
        }

        for(int j = 0; j < bitsThisLoop && bitIndex < bnum; ++j) {

            u.pduData.loopData.Circuits[i].current += u.pduData.outputData.outputBits[bitIndex].current;
            u.pduData.loopData.Circuits[i].activePower += u.pduData.outputData.outputBits[bitIndex].activePower;
            u.pduData.loopData.Circuits[i].apparentPower += u.pduData.outputData.outputBits[bitIndex].apparentPower;
            u.pduData.loopData.Circuits[i].reactivePower += u.pduData.outputData.outputBits[bitIndex].reactivePower;
            u.pduData.loopData.Circuits[i].energy += u.pduData.outputData.outputBits[bitIndex].energy;

            bitIndex++;
        }

        if(u.pduData.loopData.Circuits[i].current > u.pduData.loopData.Circuits[i].curAlarmMax)
            u.pduData.loopData.Circuits[i].curAlarmStatus = 8;
        u.pduData.loopData.Circuits[i].powerFactor =
            (u.pduData.loopData.Circuits[i].apparentPower != 0 ? u.pduData.loopData.Circuits[i].activePower/u.pduData.loopData.Circuits[i].apparentPower : 0);

    }

}

void mp_bulksend::bitInti(PowerSystemData& u)
{
    int size = u.pduData.outputData.outputBits.size();
    for(int i = 0;i < size;i ++){
        u.pduData.outputData.outputBits[i].curAlarmMax = u.thresholds.capbitCur;
        u.pduData.outputData.outputBits[i].current = specRanNumGgen::getrandom(300);
        u.pduData.outputData.outputBits[i].vol = u.thresholds.capVol;
        u.pduData.outputData.outputBits[i].powerFactor = specRanNumGgen::getrandom(100);
        u.pduData.outputData.outputBits[i].activePower = data_cal::active_powerCal(u.pduData.outputData.outputBits[i].vol,u.pduData.outputData.outputBits[i].current,u.pduData.outputData.outputBits[i].powerFactor);
        u.pduData.outputData.outputBits[i].apparentPower = data_cal::apparent_powerCal(u.pduData.outputData.outputBits[i].vol,u.pduData.outputData.outputBits[i].current);
        u.pduData.outputData.outputBits[i].reactivePower = data_cal::reactive_powerCal(u.pduData.outputData.outputBits[i].apparentPower,u.pduData.outputData.outputBits[i].activePower);
        u.pduData.outputData.outputBits[i].curAlarmMax = 16;
    }
}

void mp_bulksend::envInti(PowerSystemData& packet)
{
    int size = packet.pduData.envData.dewPoints.size();
    for(int i = 0;i < size; i++){
        packet.pduData.envData.sensorStatuses[i] = 1;
        packet.pduData.envData.humidities[i] = specRanNumGgen::get_humidity();
        packet.pduData.envData.temperatures[i] = specRanNumGgen::get_temperature();
        packet.pduData.envData.dewPoints[i] = data_cal::calculate_dewpoint1(packet.pduData.envData.temperatures[i],  packet.pduData.envData.humidities[i]);
    }

}

void mp_bulksend::dataResize(PowerSystemData & packet)
{
    packet.pduData.outputData.outputBits.resize(bitNum);
    packet.pduData.loopData.Circuits.resize(cirNum);
    packet.pduData.phases.initialize(0);
    packet.pduData.envData.initialize(4);
}

void mp_bulksend::saveSettings(QSettings &settings) {
    settings.beginGroup("Mpbulk");
    SettingsHelper::saveSpinBox(settings, "AsendNum", ui->AsendNum);
    SettingsHelper::saveSpinBox(settings, "BsendNum", ui->BsendNum);
    SettingsHelper::saveSpinBox(settings, "CsendNum", ui->CsendNum);
    SettingsHelper::saveSpinBox(settings, "DsendNum", ui->DsendNum);
    settings.endGroup();
}

void mp_bulksend::loadSettings(QSettings &settings) {
    settings.beginGroup("Mpbulk");
    SettingsHelper::loadSpinBox(settings, "AsendNum", ui->AsendNum);
    SettingsHelper::loadSpinBox(settings, "BsendNum", ui->BsendNum);
    SettingsHelper::loadSpinBox(settings, "CsendNum", ui->CsendNum);
    SettingsHelper::loadSpinBox(settings, "DsendNum", ui->DsendNum);
    settings.endGroup();
}


