// tmap_processor.cpp
#include "TMapProcessor.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <math.h>
#include "specrannumggen.h"
#include "data_cal/data_cal.h"

TMapProcessor::TMapProcessor(QObject* parent)
    : QThread(parent)
{
   // qDebug() << "TProcessor created";
}

TMapProcessor::~TMapProcessor()
{
    m_running = false;
    wait();
    quit();
}

void TMapProcessor::run() {
    while(m_running){
        QDateTime t1 = QDateTime::currentDateTime();
        for (auto it = tMap.begin(); it != tMap.end(); ++it) {

            Incchange(it.value());    // 增量变化计算
            EleCal(it.value());       // 电量计算
            PowerCal(it.value());     // 功率计算
            it.value().totalDataCal();// 总体数据计算

            auto u = toJson(it.value());
            {
                QMutexLocker locker(&TQueueMutex);  // 自动加锁解锁
                TJsonQueue.enqueue(u);
            }
        }

        QDateTime t2 = QDateTime::currentDateTime();
        int duration = t1.msecsTo(t2);
      //  qDebug()<<"duration: "<<duration<<"  "<<Stimesend;
        if(duration<=Stimesend*1000)
            msleep(Stimesend*1000-duration);
        msleep(1);
    }
}

void TMapProcessor::Tchangerun(bool flag)
{
    if(flag)m_running = 1;
    else m_running = 0;
}

void TMapProcessor::EleCal(IP_sDataPacket<3>&v)
{
    QString x = v.datetime;
    QString y = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    v.datetime = y;
    int k = data_cal::calculateTimeDiffInSeconds(y,x);
   // qDebug()<<x<<'\n'<<y<<'\n'<<k<<"????";
    for(int i=0;i<3;i++){
        v.line_item.ele_active[i] += (v.line_item.pow_value[i]*k)/3600;
    }
}

void TMapProcessor::PowerCal(IP_sDataPacket<3>&v)
{
    for(int i=0;i<3;i++){  //计算
        v.line_item.pow_apparent[i] = data_cal::
            apparent_powerCal(v.line_item.vol_value[i],v.line_item.cur_value[i]);
        v.line_item.pow_value[i] = data_cal::
            active_powerCal(v.line_item.vol_value[i],v.line_item.cur_value[i],v.line_item.power_factor[i]);
        v.line_item.pow_reactive[i] = data_cal::
            reactive_powerCal(v.line_item.pow_apparent[i],v.line_item.pow_value[i]);
    }

}

void TMapProcessor::Incchange(IP_sDataPacket<3>&v)
{


    {   //温度增量
        double x = specRanNumGgen::getrandom(100);
        double y = v.env_item.tem_value[0];
        double newVal = v.incrEnvInc ? x + y : x - y;
        v.env_item.tem_value[0] = ((v.incrEnvInc ? fmin(x + y, 100) : fmax(x - y, 0)));
        v.incrEnvInc = (v.incrEnvInc && newVal >= 100) ? false :
                           (!v.incrEnvInc && newVal <= 0)   ? true  :
                           v.incrEnvInc;
    }
    {   //湿度增量
        double x = specRanNumGgen::getrandom(100);
        double y = v.env_item.hum_value[0];
        double newVal = v.incrEnvHum ? x + y : x - y;
        v.env_item.hum_value[0] = ((v.incrEnvHum ? fmin(x + y, 100) : fmax(x - y, 0)));
        v.incrEnvHum = (v.incrEnvHum && newVal >= 100) ? false :
                           (!v.incrEnvHum && newVal <= 0)   ? true  :
                           v.incrEnvHum;
       // qDebug()<<y<<"   "<<v.env_item.hum_value[0];

        v.env_item.dew_point[0] = data_cal::calculate_dewpoint1(v.env_item.tem_value[0], v.env_item.hum_value[0]);
    }

    {   //电压增量
        updatePhaseValue(v.line_item.vol_value[0], v.volIncA);
        updatePhaseValue(v.line_item.vol_value[1], v.volIncB);
        updatePhaseValue(v.line_item.vol_value[2], v.volIncC);
    }

    {   //电流增量
        updateCurrentPhase(v.line_item.cur_value[0], v.curIncA, v.line_item.cur_alarm_max[0]);
        updateCurrentPhase(v.line_item.cur_value[1], v.curIncB, v.line_item.cur_alarm_max[1]);
        updateCurrentPhase(v.line_item.cur_value[2], v.curIncC, v.line_item.cur_alarm_max[2]);
    }

    {   //功率增量
        updatePowerFactor(v.line_item.power_factor[0], v.pfIncA);
        updatePowerFactor(v.line_item.power_factor[1], v.pfIncB);
        updatePowerFactor(v.line_item.power_factor[2], v.pfIncC);
    }

}

void TMapProcessor::updatePowerFactor(double& pf, bool& incFlag) {
    constexpr double PF_MAX = 1.0;    // 上限
    constexpr double PF_MIN = 0.1;    // 下限
    constexpr double EPSILON = 1e-6;  // 浮点比较容差


    double y = specRanNumGgen::getrandom(30);
    double newval = incFlag ? (pf + y) : (pf - y);


    pf = incFlag ? fmin(newval, PF_MAX) : fmax(newval, PF_MIN);


    bool isOverMax = newval >= PF_MAX - EPSILON;
    bool isUnderMin = newval <= PF_MIN + EPSILON;


    if (incFlag && isOverMax) {
        incFlag = false;  // 达到上限转递减
    } else if (!incFlag && isUnderMin) {
        incFlag = true;   // 达到下限转递增
    }
}

void TMapProcessor::updateCurrentPhase(double& value, bool& incFlag, double cap){
    double x = specRanNumGgen::getrandom(100.0);
    double newval = incFlag ? (value + x) : (value - x);

    value = incFlag ? fmin(newval, cap) : fmax(newval, 0.0);
    incFlag = (incFlag && newval >= cap) ? false :
                  (!incFlag && newval <= 0)  ? true  :
                  incFlag;
}

void TMapProcessor::updatePhaseValue(double& value, bool& incFlag) {
    double x = specRanNumGgen::getrandom(100);
    double y = value;
    double newval = incFlag ? (y + x) : (y - x);

    // 限制范围并更新值
    value = incFlag ? fmin(newval, 276.0) : fmax(newval, 250.0);

    // 更新增减模式
    if (incFlag && newval >= 276.0) {
        incFlag = false;
    } else if (!incFlag && newval <= 250.0) {
        incFlag = true;
    }
}

