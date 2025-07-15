#include "smapprocessor.h"
#include "specrannumggen.h"
#include "data_cal/data_cal.h"
#include "databasemanager.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <math.h>
SMapProcessor::SMapProcessor(QObject* parent)
    : QThread(parent)
{
   // qDebug() << "SProcessor created";
    mSaveTimer = new QTimer(this);
    dbWriteThread = new DbWriteThread(this);
    dbWriteThread->start();

    mSaveTimer->setInterval(40 * 60 * 1000); // 5分钟一次
    connect(mSaveTimer, &QTimer::timeout, this, &SMapProcessor::onSaveTimerTimeout);
    mSaveTimer->start();

}

SMapProcessor::~SMapProcessor()
{
    m_running = false;
    wait();
    quit();
}

void SMapProcessor::run()
{

    while(m_running){

       QDateTime t1 = QDateTime::currentDateTime();
        for (auto it = sMap.begin(); it != sMap.end(); ++it) {

           auto &dev = it.value();

            Incchange(dev);    // 增量变化计算
            EleCal(dev,it->dev_key);       // 电量计算
            PowerCal(dev);     // 功率计算
            dev.totalDataCal();// 总体数据计算

            auto u = toJson(dev);
            {
                QMutexLocker locker(&SQueueMutex);  // 自动加锁解锁

                SJsonQueue.enqueue(u);
            }
        }


        QDateTime t2 = QDateTime::currentDateTime();
        int duration = t1.msecsTo(t2);
       // qDebug()<<"duration: "<<duration<<"  "<<Stimesend;
        if(duration<=Stimesend*1000){
            msleep(Stimesend*1000-duration);
        }
        //qDebug()<<duration<<"  "<<SCnt<<"  "<<SCntEr<<endl;
        emit ScheckSend(duration,SCnt,SCntEr);
        SCnt = SCntEr = 0;

    }
}

void SMapProcessor::Incchange(IP_sDataPacket<1>&v)
{


    {   //温度增量
        double x = specRanNumGgen::getrandom(100);
        double y = v.env_item.tem_value[0];
        double newVal = v.incrEnvInc ? x + y : x - y;
        v.env_item.tem_value[0] = ((v.incrEnvInc ? fmin(x + y, 45) : fmax(x - y, 20)));
        v.incrEnvInc = (v.incrEnvInc && newVal >= 45) ? false :
                           (!v.incrEnvInc && newVal <= 20)   ? true  :
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
       //电压增量
        updatePhaseValue(v.line_item.vol_value[0], v.volIncA);

       //电流增量
        updateCurrentPhase(v.line_item.cur_value[0], v.curIncA, v.line_item.cur_alarm_max[0]);


       //功率增量
        updatePowerFactor(v.line_item.power_factor[0], v.pfIncA);
}

void SMapProcessor::updatePowerFactor(double& pf, bool& incFlag) {
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

void SMapProcessor::updateCurrentPhase(double& value, bool& incFlag, double cap){
    double x = specRanNumGgen::getrandom(100.0);
    double newval = incFlag ? (value + x) : (value - x);

    value = incFlag ? fmin(newval, cap) : fmax(newval, 0.0);
    incFlag = (incFlag && newval >= cap) ? false :
                  (!incFlag && newval <= 0)  ? true  :
                  incFlag;
}

void SMapProcessor::updatePhaseValue(double& value, bool& incFlag) {
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

void SMapProcessor::EleCal(IP_sDataPacket<1>& v, const QString &key) // 加 const QString &key 参数
{
    QString x = v.datetime;
    QString y = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    v.datetime = y;
    int k = data_cal::calculateTimeDiffInSeconds(y, x);
    // qDebug() << x << '\n' << y << '\n' << k << "????";

    v.line_item.ele_active[0] += (v.line_item.pow_value[0] * k) / 3600;
    // qDebug() << v.line_item.ele_active[0] << " " << v.line_item.pow_value[0] * k;

    // === 新增：计算后立即保存数据库 ===
  //   bool saveOk = DatabaseManager::instance().insertOrUpdateSignalPhaseEnergy(
  //       key,
  //       v.line_item.ele_active[0]
  //       );

  // //  if (!saveOk) {
  // //      qDebug() << "保存单相电能失败:" << key;
  // //  } else {
  // //      qDebug() << "保存单相电能成功:" << key << v.line_item.ele_active[0];
  // // }
}

void SMapProcessor::onSaveTimerTimeout()
{
    QMutexLocker locker(&mMapMutex);

    for (auto it = sMap.begin(); it != sMap.end(); ++it) {
        DbWriteTask task;
        task.table = DbWriteTask::SinglePhase; // 根据你的业务修改
        task.key = it.key();
        task.values.append(it.value().line_item.ele_active[0]);

        dbWriteThread->enqueueTask(task);  // 这里仅入队，不阻塞
    }

    qDebug() << "本次 onSaveTimerTimeout 入队完成";
}

void SMapProcessor::PowerCal(IP_sDataPacket<1>&v)
{
    for(int i=0;i<1;i++){  //计算
        v.line_item.pow_apparent[i] = data_cal::
            apparent_powerCal(v.line_item.vol_value[i],v.line_item.cur_value[i]);
        v.line_item.pow_value[i] = data_cal::
            active_powerCal(v.line_item.vol_value[i],v.line_item.cur_value[i],v.line_item.power_factor[i]);
        v.line_item.pow_reactive[i] = data_cal::
            reactive_powerCal(v.line_item.pow_apparent[i],v.line_item.pow_value[i]);
    }

}

void SMapProcessor::Schangerun(bool flag)
{
    m_running = flag;
}
