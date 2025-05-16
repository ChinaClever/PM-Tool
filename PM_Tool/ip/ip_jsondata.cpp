#include "ip_jsondata.h"
#include "ui_ip_jsondata.h"
#include "backcolourcom.h"
#include "specrannumggen.h"
#include "data_cal/data_cal.h"

#include <QButtonGroup>
#include <QDebug>
#include <math.h>
#include <QJsonArray>
#include <QMessageBox>
IP_JsonData::IP_JsonData(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::IP_JsonData)
{
    ui->setupUi(this);
    set_background_icon(this,":/image/box_back.jpg");
    intiSec();
    connect(
        BtnGroup_phase,
        &QButtonGroup::idClicked,
        this,
        &IP_JsonData::ST_Switch_Com_Changed
        );//单三相电压切换
    connect(
        BtnGroup_ele,
        &QButtonGroup::idClicked,
        this,
        &IP_JsonData::set_current
        ); //电流切换
    connect(this, &IP_JsonData::phaseChangeSig, this, &IP_JsonData::set_abled);
    timeinti();
  //  ip_sendJson Json; //定义json对象
    ip_sendJson *Json = new(ip_sendJson);
    connect(this,&IP_JsonData::sendSig,Json,&ip_sendJson::sendTogoal);
}

IP_JsonData::~IP_JsonData()
{
    delete ui;
}

void IP_JsonData::timeinti()
{
    QList<QDoubleSpinBox*> spinBoxes = {
        ui->volA,ui->volB,ui->volC,ui->curA,ui->curB,ui->curC,
        ui->pfA,ui->pfB,ui->pfC,ui->eleA,ui->eleB,ui->eleC,
        ui->envHum,ui->envTem
    };
    for (QDoubleSpinBox* spinBox : spinBoxes) {
        connect(
            spinBox,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            &IP_JsonData::powerdataCal
            );
        connect(
            spinBox,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            &IP_JsonData::totalData
            );
        connect(
            spinBox,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            &IP_JsonData::alarmturn //运行时改变值直接发送json(函数内实现)
            );
    }

    ui->volA->setValue(specRanNumGgen::generate_voltage_double());//触发第一次值更新

    incr_timer = new QTimer(this);  //修改增量因数
    incr_timer->setInterval(ui->timeInv->value()*1000);
    incr_timer->setSingleShot(false);
    connect(incr_timer, &QTimer::timeout, this, &IP_JsonData::incrCal);
    incr_timer->start(); // 启动定时器

    timesend = new QTimer(this);
    timesend->setInterval(10000);

    connect(ui->timeInv,
            QOverload<int>::of(&QSpinBox::valueChanged),
            this,
            &IP_JsonData::timechange);
    connect(timesend,&QTimer::timeout,this,&IP_JsonData::timeoutsend);


}

void IP_JsonData::intiSec() //IP启动初始化
{
    ui->devIp->setValidator(new QRegExpValidator(QRegExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b")));
    ui->serIp->setValidator(new QRegExpValidator(QRegExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b")));
    ui->port->setValidator(new QIntValidator(0, 65532,ui->port));


    BtnGroup_phase = new QButtonGroup;
    BtnGroup_ele = new QButtonGroup;
    BtnGroup_phase->addButton(ui->tpe,0);  //三相
    BtnGroup_phase->addButton(ui->spe,1);   //单相
    ui->tpe->setChecked(true);

    BtnGroup_ele->addButton(ui->cur32A,0);
    BtnGroup_ele->addButton(ui->cur63A,1);
    BtnGroup_ele->addButton(ui->cur125A,2);
    ui->cur63A->setChecked(true);

    ui->volA->setValue(specRanNumGgen::generate_voltage_double());
    ui->volB->setValue(specRanNumGgen::generate_voltage_double());
    ui->volC->setValue(specRanNumGgen::generate_voltage_double());

    ui->curA->setValue(specRanNumGgen::generate_phase_current(63));
    ui->curB->setValue(specRanNumGgen::generate_phase_current(63));
    ui->curC->setValue(specRanNumGgen::generate_phase_current(63));


    ui->pfA->setValue(specRanNumGgen::get_power_factor_precise());
    ui->pfB->setValue(specRanNumGgen::get_power_factor_precise());
    ui->pfC->setValue(specRanNumGgen::get_power_factor_precise());

    ui->envTem->setValue(specRanNumGgen::get_temperature());
    ui->envHum->setValue(specRanNumGgen::get_humidity());
}

void IP_JsonData::ST_Switch_Com_Changed(int index)
{
    /*
     0：三相  显示BC相控件
     1：单相  隐藏BC相控件
     */
    if(index == 0){
        emit phaseChangeSig(1);
    }else{
        emit phaseChangeSig(0);
    }
}


void IP_JsonData::on_sendJsonBtn_clicked()
{
    if(ui->sendJsonBtn->text()=="开始发送"){
        jsonassignment();
        emit sendSig(Json.getJsonObject(),ui->serIp->text(),ui->port->text());
        timesend->start();
        ui->sendJsonBtn->setText("停止发送");
    }
    else{
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this,
                                      "确认停止",
                                      "确定要停止发送数据吗？",
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            timesend->stop();
            ui->sendJsonBtn->setText("开始发送");
            qDebug() << "已停止数据发送";
        } else {
            // 用户取消，保持发送状态
            qDebug() << "用户取消了停止操作";
        }

    }

}
void IP_JsonData::timechange()
{
    // 停止定时器
    if (timesend->isActive()) {
        timesend->stop();
        qDebug() << "Timer stopped";
    }

    // 设置新间隔
    const int newInterval = ui->timeInv->value()*1000;
    timesend->setInterval(newInterval);
    if(ui->sendJsonBtn->text()=="停止发送"){
        timesend->start();
    }
    qDebug() << "Timer restarted with interval:" << newInterval;
}
void IP_JsonData::timeoutsend()
{
    int phase = BtnGroup_phase->checkedId();    //时间间隔到，更新有功电能
    ui->eleA->setValue(ui->eleA->value()+(ui->act_PowA->value()*(ui->timeInv->value()/3600)));
    if(phase){
        ui->eleB->setValue(ui->eleA->value()+(ui->act_PowB->value()*(ui->timeInv->value()/3600)));
        ui->eleC->setValue(ui->eleA->value()+(ui->act_PowC->value()*(ui->timeInv->value()/3600)));
    }

    qDebug()<<222;
    jsonassignment();
    emit sendSig(Json.getJsonObject(),ui->serIp->text(),ui->port->text());
}
void IP_JsonData::jsonassignment()
{

    int addr = ui->addr->text().toInt();
    int status = 0;
    QString devIp = ui->devIp->text().trimmed();
    QString datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString pdu_alarm = "";
    QJsonObject envItemlist;
    QJsonObject lineItemList;
    QJsonObject pduTotaldata;
    QJsonObject pduData;

    envItemlist = getenvItemlist();
    Json.setEnvItemList(envItemlist);
        // qDebug()<<envItemlist;

    lineItemList = getlineItemList();
    Json.setLineItemList(lineItemList);
    //qDebug()<<lineItemList;

    pduTotaldata = getpduTotaldata();
    Json.setPduTotalData(pduTotaldata);
    //qDebug()<<pduTotaldata;
    pduData = getpduData(envItemlist,lineItemList,pduTotaldata);
    Json.generatePduData(pduData);
    //qDebug()<<pduData;

    status = (statusflag?0:2);
    Json.setStatus(status);

    pdu_alarm = getAlarm();
    Json.setAlarm(pdu_alarm);
    //qDebug()<<pdu_alarm;

    Json.setSerIP(ui->serIp->text()); //目标ip
    Json.setPort(ui->port->text());  //目标端口

    Json.setDatetime(datetime);
    Json.setAddr(addr);
    Json.setDevIp(devIp);
    Json.setDevkey();

    Json.generateJson();

}
void IP_JsonData::set_current(int index)
{
    double cur  = (index == 0) ? 32 : (index == 1) ? 63 : 125;

    ui->curCapA->setValue(cur);
    ui->curCapB->setValue(cur);
    ui->curCapC->setValue(cur);


}

void IP_JsonData::set_abled(int id)
{
    ui->volB->setEnabled(id);
    ui->volC->setEnabled(id);
    ui->curB->setEnabled(id);
    ui->curC->setEnabled(id);
    ui->pfB->setEnabled(id);
    ui->pfC->setEnabled(id);
    ui->eleB->setEnabled(id);
    ui->eleC->setEnabled(id);

    ui->volUnbal->setVisible(id);
    ui->curUnbal->setVisible(id);
    ui->vol_unbal_4->setVisible(id);
    ui->cur_unbal_4->setVisible(id);
}
void IP_JsonData::powerdataCal()
{
    int phase = BtnGroup_phase->checkedId();
    //qDebug()<<phase;

    //ui->act_PowA->setValue();
    ui->ApparentA->setValue(data_cal::apparent_powerCal(ui->volA->value(),ui->curA->value()));
    ui->act_PowA->setValue(data_cal::active_powerCal(ui->volA->value(),ui->curA->value(),ui->pfA->value()));
    ui->Rec_PowA->setValue(data_cal::reactive_powerCal(ui->ApparentA->value(),ui->act_PowA->value()));

    if(!phase){

        ui->ApparentB->setValue(data_cal::apparent_powerCal(ui->volB->value(),ui->curB->value()));
        ui->ApparentC->setValue(data_cal::apparent_powerCal(ui->volC->value(),ui->curC->value()));

        ui->act_PowB->setValue(data_cal::active_powerCal(ui->volA->value(),ui->curB->value(),ui->pfB->value()));
        ui->act_PowC->setValue(data_cal::active_powerCal(ui->volA->value(),ui->curC->value(),ui->pfC->value()));

        ui->Rec_PowB->setValue(data_cal::reactive_powerCal(ui->ApparentB->value(),ui->act_PowB->value()));
        ui->Rec_PowC->setValue(data_cal::reactive_powerCal(ui->ApparentC->value(),ui->act_PowC->value()));
    }

}

void IP_JsonData::incrCal()  //增量计算
{
    int phase = BtnGroup_phase->checkedId();
    if (double y = ui->envInc->value()) {
        double x = ui->envTem->value();
        double newVal = incrEnvInc ? x + y : x - y;
        ui->envTem->setValue(incrEnvInc ? fmin(x + y, 100) : fmax(x - y, 0));
        incrEnvInc = (incrEnvInc && newVal >= 100) ? false :
                         (!incrEnvInc && newVal <= 0)   ? true  :
                         incrEnvInc;
    }

    if(int y = ui->envhumInc->value()){
        int x = ui->envHum->value();
        int newVal = incrEnvHum ? x + y : x - y;
        ui->envHum->setValue(incrEnvHum ? fmin(x + y, 100) : fmax(x - y, 0));
        incrEnvHum = (incrEnvHum && newVal >= 100) ? false :
                         (!incrEnvHum && newVal <= 0)   ? true  :
                         incrEnvHum;
    }

    if(double y = ui->volInc->value()){
        int cap = ui->volCapA->value();
        int x = ui->volA->value();
        ui->volA->setValue(volIncA ? fmin(x+y,cap) : fmax(x-y,0));
        int newVal = volIncA ? x+y : x-y;
        volIncA = (volIncA && newVal >= cap) ? false :
                      (!volIncA && newVal <= 0)   ? true  :
                      volIncA;
    }

    if(double y = ui->curInc->value()){
        int cap = ui->curCapA->value();
        int x = ui->curA->value();
        int newVal = curIncA ? x+y : x-y;
        ui->curA->setValue(curIncA ? fmin(x+y,cap) : fmax(x-y,0));
        curIncA = (curIncA && newVal >= cap) ? false :
                      (!curIncA && newVal <= 0)   ? true  :
                      curIncA;
    }

    if(double y = ui->pfInc->value()){
        double x = ui->pfA->value();
        double newVal = pfIncA ? x+y : x-y;
        ui->pfA->setValue(pfIncA ? fmin(x+y,1) : fmax(x-y,0));
        pfIncA = (pfIncA && newVal >= 1) ? false :
                      (!pfIncA && newVal <= 0)   ? true  :
                      pfIncA;
    }

    if(double y = ui->eleInc->value()){
        double x = ui->eleA->value();
        ui->eleA->setValue(x+y);
    }


    if(!phase){
        if(double y = ui->volInc->value()){
            int cap = ui->volCapB->value();
            int x = ui->volB->value();
            int newVal = volIncB ? x+y : x-y;
            ui->volB->setValue(volIncB ? fmin(x+y,cap) : fmax(x-y,0));
            volIncB = (volIncB && newVal >= cap) ? false :
                          (!volIncB && newVal <= 0)   ? true  :
                          volIncB;
        }
        if(double y = ui->volInc->value()){
            int cap = ui->volCapC->value();
            int x = ui->volC->value();
            int newVal = volIncC ? x+y : x-y;
            ui->volC->setValue(volIncC ? fmin(x+y,cap) : fmax(x-y,0));
            volIncC = (volIncC && newVal >= cap) ? false :
                          (!volIncC && newVal <= 0)   ? true  :
                          volIncC;
        }
        if(double y = ui->curInc->value()){
            int cap = ui->curCapB->value();
            int x = ui->curB->value();
            int newVal = curIncB ? x+y : x-y;
            ui->curB->setValue(curIncB ? fmin(x+y,cap) : fmax(x-y,0));
            curIncB = (curIncB && newVal >= cap) ? false :
                          (!curIncB && newVal <= 0)   ? true  :
                          curIncB;
        }
        if(double y = ui->curInc->value()){
            int cap = ui->curCapC->value();
            int x = ui->curC->value();
            int newVal = curIncC ? x+y : x-y;
            ui->curC->setValue(curIncC ? fmin(x+y,cap) : fmax(x-y,0));
            curIncC = (curIncC && newVal >= cap) ? false :
                          (!curIncC && newVal <= 0)   ? true  :
                          curIncC;
        }
        if(double y = ui->pfInc->value()){
            double x = ui->pfB->value();
            double newVal = pfIncB ? x+y : x-y;
            ui->pfB->setValue(pfIncB ? fmin(x+y,1) : fmax(x-y,0));
            pfIncB = (pfIncB && newVal >= 1) ? false :
                         (!pfIncB && newVal <= 0)   ? true  :
                         pfIncB;
        }
        if(double y = ui->pfInc->value()){
            double x = ui->pfC->value();
            double newVal = pfIncC ? x+y : x-y;
            ui->pfC->setValue(pfIncC ? fmin(x+y,1) : fmax(x-y,0));
            pfIncC = (pfIncC && newVal >= 1) ? false :
                         (!pfIncC && newVal <= 0)   ? true  :
                         pfIncC;
        }

        if(double y = ui->eleInc->value()){
            double x = ui->eleB->value();
            ui->eleB->setValue(x+y);
        }
        if(double y = ui->eleInc->value()){
            double x = ui->eleC->value();
            ui->eleC->setValue(x+y);
        }
    }
}


void IP_JsonData::alarmturn()
{
    if(ui->curA->value()>=ui->curCapA->value()){
        ui->curA->setStyleSheet("color: red;");
    }
    else{
        ui->curA->setStyleSheet("font-family: 微软雅黑;");
    }
    if(ui->curB->value()>=ui->curCapB->value()){
        ui->curB->setStyleSheet("color: red;");
    }
    else{
        ui->curB->setStyleSheet("font-family: 微软雅黑;");
    }
    if(ui->curC->value()>=ui->curCapC->value()){
        ui->curC->setStyleSheet("color: red;");
    }
    else{
        ui->curC->setStyleSheet("font-family: 微软雅黑;");
    }

    if(ui->volA->value()>=ui->volCapA->value()){
        ui->volA->setStyleSheet("color: red;");
    }
    else{
        ui->volA->setStyleSheet("font-family: 微软雅黑;");
    }
    if(ui->volB->value()>=ui->volCapB->value()){
        ui->volB->setStyleSheet("color: red;");
    }
    else{
        ui->volB->setStyleSheet("font-family: 微软雅黑;");
    }
    if(ui->volC->value()>=ui->volCapC->value()){
        ui->volC->setStyleSheet("color: red;");
    }
    else{
        ui->volC->setStyleSheet("font-family: 微软雅黑;");
    }
    if(ui->sendJsonBtn->text()=="停止发送"){
        timesend->stop();
        jsonassignment();
        emit sendSig(Json.getJsonObject(),ui->serIp->text(),ui->port->text());
        timesend->start(ui->timeInv->value()*1000);
    }
}



void IP_JsonData::totalData()  //统计数据
{
    int phase = BtnGroup_phase->checkedId();    //单三相
    double actPowTotal = ui->act_PowA->value();
    double powReactTotal = ui->Rec_PowA->value();
    double totalPaTotal = ui->ApparentA->value();
    double totalEleact = ui->eleA->value();
    if(!phase){
        actPowTotal += ui->act_PowB->value();
        actPowTotal += ui->act_PowC->value();
        powReactTotal += ui->Rec_PowB->value();
        powReactTotal += ui->Rec_PowC->value();
        totalPaTotal += ui->ApparentB->value();
        totalPaTotal += ui->ApparentC->value();
        totalEleact += ui->eleB->value();
        totalEleact += ui->eleC->value();

        {
            double x =(std::max({ui->volA->value(),ui->volB->value(),ui->volC->value()})
                        -std::min({ui->volA->value(),ui->volB->value(),ui->volC->value()}));
            double y = (ui->volA->value()+ui->volB->value()+ui->volC->value())/3.0;
            ui->volUnbal->setValue(y == 0 ? 0 : (x / y * 1.0)*100);

            x =(std::max({ui->curA->value(),ui->curB->value(),ui->curC->value()})
                 -std::min({ui->curA->value(),ui->curB->value(),ui->curC->value()}));
            y = (ui->curA->value()+ui->curB->value()+ui->curC->value())/3.0;
            ui->curUnbal->setValue(y == 0 ? 0 : (x / y * 1.0)*100);

            //   qDebug()<<x<<"   "<<y;
        }
    }
    //qDebug()<<actPowTotal<<"  "<<totalPaTotal;

    ui->totalPf->setValue(totalPaTotal == 0 ? 0 : (actPowTotal / totalPaTotal * 1.0));
    ui->powAct->setValue(actPowTotal);
    ui->powReact->setValue(powReactTotal);
    ui->totalPA->setValue(totalPaTotal);
    ui->totalEleact->setValue(totalEleact);
}


QJsonObject IP_JsonData::getenvItemlist()
{
    QJsonObject envItem;


    int insert[2] = {1, 0};
    double hum[2] = {ui->envHum->value(), 0};
    double tem[2] = {ui->envTem->value(), 0};
    int temAlarm[2] = {0, 0};
    int humAlarm[2] = {0, 0};
    double dew_point[2] = {data_cal::calculate_dewpoint1(ui->envTem->value(),ui->envHum->value()),0};
    // 转换为JSON数组
    envItem["insert"] = QJsonArray::fromVariantList({insert[0], insert[1]});
    envItem["hum_value"] = QJsonArray::fromVariantList({hum[0], hum[1]});
    envItem["tem_value"] = QJsonArray::fromVariantList({tem[0], tem[1]});
    envItem["dew_point"] = QJsonArray::fromVariantList({dew_point[0], dew_point[1]});
    envItem["hum_alarm_status"] = QJsonArray::fromVariantList({humAlarm[0], humAlarm[1]});
    envItem["tem_alarm_status"] = QJsonArray::fromVariantList({temAlarm[0], temAlarm[1]});

    return envItem;
}

QJsonObject IP_JsonData::getlineItemList()
{
    QJsonObject lineItem;
    int phase = BtnGroup_phase->checkedId();

    // 定义数据容器
    QVector<double> vol_value  = {ui->volA->value()};
    QVector<double> cur_value  = {ui->curA->value()};
    QVector<double> ele_active = {ui->eleA->value()};
    QVector<double> power_factor = {ui->pfA->value()};
    QVector<double> cur_alarm_max = {ui->curCapA->value()};
    QVector<double> vol_alarm_max = {ui->volCapA->value()};
    QVector<double> pow_value = {ui->act_PowA->value()};
    QVector<double> pow_apparent = {ui->ApparentA->value()};
    QVector<double> pow_reactive = {ui->Rec_PowA->value()};

    QVector<int> vol_alarm_status = {0};
    QVector<int> cur_alarm_status = {0};
    QVector<int> pow_alarm_status = {0};

    // 多相处理
    if (!phase) {
        // B相数据
        vol_value.push_back(ui->volB->value());
        cur_value.push_back(ui->curB->value());
        ele_active.push_back(ui->eleB->value());
        power_factor.push_back(ui->pfB->value());
        cur_alarm_max.push_back(ui->curCapB->value());
        vol_alarm_max.push_back(ui->volCapB->value());
        pow_value.push_back(ui->act_PowB->value());
        pow_apparent.push_back(ui->ApparentB->value());
        pow_reactive.push_back(ui->Rec_PowB->value());

        // C相数据
        vol_value.push_back(ui->volC->value());
        cur_value.push_back(ui->curC->value());
        ele_active.push_back(ui->eleC->value());
        power_factor.push_back(ui->pfC->value());
        cur_alarm_max.push_back(ui->curCapC->value());
        vol_alarm_max.push_back(ui->volCapC->value());
        pow_value.push_back(ui->act_PowC->value());
        pow_apparent.push_back(ui->ApparentC->value());
        pow_reactive.push_back(ui->Rec_PowC->value());

        // 初始化三相报警状态
        vol_alarm_status = {0, 0, 0};
        cur_alarm_status = {0, 0, 0};
        pow_alarm_status = {0, 0, 0};
    }

    // 报警状态检测
    const int phaseCount = phase ? 1 : 3; // 确定相位数量
    statusflag = true;
    for(int i = 0; i < phaseCount; ++i) {
        // 电流报警检测
        if(cur_value[i] > cur_alarm_max[i]) {
            statusflag = false;;
            cur_alarm_status[i] = 8;
            qDebug() << QString("Phase %1电流超标：%2 > %3")
                            .arg(char('A' + i))
                            .arg(cur_value[i])
                            .arg(cur_alarm_max[i]);
        }

        // 电压报警检测
        if(vol_value[i] > vol_alarm_max[i]) {
            statusflag = false;
            vol_alarm_status[i] = 8;
            qDebug() << QString("Phase %1电压超标：%2 > %3")
                            .arg(char('A' + i))
                            .arg(vol_value[i])
                            .arg(vol_alarm_max[i]);
        }
    }

    // 转换为 JSON 数组的lambda
    auto toJsonArray = [](const auto& vec) {
        QJsonArray arr;
        for (const auto& val : vec) {
            arr.append(val);
        }
        return arr;
    };

    // 填充 JSON 对象
    lineItem["vol_value"] = toJsonArray(vol_value);
    lineItem["cur_value"] = toJsonArray(cur_value);
    lineItem["ele_active"] = toJsonArray(ele_active);
    lineItem["power_factor"] = toJsonArray(power_factor);
    lineItem["cur_alarm_max"] = toJsonArray(cur_alarm_max);
    lineItem["vol_alarm_max"] = toJsonArray(vol_alarm_max);
    lineItem["pow_value"] = toJsonArray(pow_value);
    lineItem["pow_apparent"] = toJsonArray(pow_apparent);
    lineItem["pow_reactive"] = toJsonArray(pow_reactive);

    lineItem["vol_alarm_status"] = toJsonArray(vol_alarm_status);
    lineItem["cur_alarm_status"] = toJsonArray(cur_alarm_status);
    lineItem["pow_alarm_status"] = toJsonArray(pow_alarm_status);

    // 添加相位标识
   // lineItem["phase_type"] = phase ? "single" : "three";

    return lineItem;
}
QJsonObject IP_JsonData::getpduTotaldata()
{
    QJsonObject pdu_total_data;
    const int phaseType = BtnGroup_phase->checkedId(); // 0=三相，1=单相

    // 公共数据项
    const double ele_active = ui->totalEleact->value();
    const double pow_active = ui->powAct->value();
    const double pow_apparent = ui->totalPA->value();
    const double power_factor = (pow_apparent != 0) ? pow_active / pow_apparent : 0.0;
    const double pow_reactive = ui->powReact->value();

    // 填充公共字段
    pdu_total_data["ele_active"] = ele_active;
    pdu_total_data["pow_active"] = pow_active;
    pdu_total_data["pow_apparent"] = pow_apparent;
    pdu_total_data["power_factor"] = qRound(power_factor * 1000) / 1000.0; // 保留3位小数
    pdu_total_data["pow_reactive"] = pow_reactive;

    // 仅三相系统记录不平衡度
    if (phaseType == 0) {  // 假设0表示三相
        const double vol_unbalance = ui->volUnbal->value();
        const double cur_unbalance = ui->curUnbal->value();

        pdu_total_data["vol_unbalance"] = vol_unbalance;
        pdu_total_data["cur_unbalance"] = cur_unbalance;
    }

    return pdu_total_data;
}
QJsonObject IP_JsonData::getpduData(const QJsonObject &envItem,const QJsonObject &lineItem,
                                    const QJsonObject &totalData)
{
    QJsonObject pduData;
    // 合并三个数据模块
    pduData["env_item_list"] = envItem;   // 环境数据
    pduData["line_item_list"] = lineItem;      // 线路数据
    pduData["pdu_total_data"] = totalData;     // 汇总数据

    return pduData;
}
QString IP_JsonData::getAlarm()
{
    QStringList alarmList;
    const int phaseType = BtnGroup_phase->checkedId(); // 0=三相，1=单相
    const int phaseCount = phaseType ? 1 : 3;

    // 电压相关数据
    const QVector<double> vol_alarm_max = {
        ui->volCapA->value(),
        ui->volCapB->value(),
        ui->volCapC->value()
    };
    const QVector<double> vol_value = {
        ui->volA->value(),
        ui->volB->value(),
        ui->volC->value()
    };

    const QVector<double> cur_alarm_max = {
        ui->curCapA->value(),
        ui->curCapB->value(),
        ui->curCapC->value()
    };
    const QVector<double> cur_value = {
        ui->curA->value(),
        ui->curB->value(),
        ui->curC->value()
    };

    // 报警检测
    for(int i = 0; i < phaseCount; ++i) {
        const int phaseNum = i + 1;
        const double voltage = vol_value[i];
        // 电压检测
        if(qFuzzyIsNull(voltage)) {
            alarmList.append(
                QString("第%1相电压过低；电压：0V").arg(phaseNum)
                );
        } else if(voltage > vol_alarm_max[i]) {
            alarmList.append(
                QString("第%1相电压过高；当前：%2V，上限：%3V")
                    .arg(phaseNum)
                    .arg(voltage, 0, 'f', 2)
                    .arg(vol_alarm_max[i], 0, 'f', 1)
                );
        }
    }
    for(int i = 0; i < phaseCount; ++i) {
        const int phaseNum = i + 1;
        const double current = cur_value[i];
        // 电流检测（仅过高）
        if(current > cur_alarm_max[i]) {
            alarmList.append(
                QString("第%1相电流过高；当前：%2A，上限：%3A")
                    .arg(phaseNum)
                    .arg(current, 0, 'f', 2)
                    .arg(cur_alarm_max[i], 0, 'f', 1)
                );
        }
    }
     return alarmList.isEmpty() ? "设备运行正常" : alarmList.join("\n");
}
