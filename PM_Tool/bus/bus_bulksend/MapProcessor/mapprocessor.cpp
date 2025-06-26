#include "mapprocessor.h"
#include "bus_globals.h"
#include "specRanNumGgen.h"
#include "busbulk.h"
#include <math.h>
#include <QDebug>

MapProcessor::MapProcessor(QObject* parent)
{
    mBus = new bus_toJson(this);
}

MapProcessor::~MapProcessor()
{

    m_running = false;
    wait();
    quit();
}

void MapProcessor::run()
{
    qDebug() << "Mpstart!";
    bulkPhase = QVector<QVector<double>>(6, QVector<double>(3, 0.0));
    auto u = std::make_unique<bus_toJson>(nullptr);
    while (m_running) {
        QDateTime t1 = QDateTime::currentDateTime();

        busMapLock.lockForRead();  // ✅ 加读锁
        for (int i = 0; i < busMap.size(); i++) {


            if((i+1)%(bulkBoxNum+1) == 0){
                 busBulk::setBusline(busMap[i+1].busData);
                 busBulk::setBusTotal(busMap[i+1].busData);
                 {
                     QMutexLocker locker(&busBulkPhase);
                     bulkPhase = QVector<QVector<double>>(6, QVector<double>(3, 0.0));
                 }
            }
            else{
                busMap[i+1].info.datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
                setBoxInc(busMap[i+1].boxData);
               busBulk::setBoxList(busMap[i+1].boxData);
            }

            QJsonObject json = u->toJson(busMap[i+1], busMap[i+1].flag);
            {
                QMutexLocker locker(&busBulkJQMutexes);
                busQueue.push_back(json);
            }
          // qDebug()<<json;
        }
        busMapLock.unlock();  // ✅ 解锁

        QDateTime t2 = QDateTime::currentDateTime();
        int duration = t1.msecsTo(t2);

        if (duration <= bus_sendTime * 1000)
            msleep(bus_sendTime * 1000 - duration);
    }

    qDebug() << "Mpstop!";
}

void MapProcessor::setBoxInc(BoxData& box)
{
    double x,newVal;
    auto &u = box.loopItemList;
    for(int i = 0; i < box.loopItemList.curValue.size(); i ++ ){
        x = specRanNumGgen::getrandom(100);
        double &y = u.curValue[i];
        newVal = u.curInc[i] ? x+y : y-x;

        y = ((u.curInc[i] ? fmin(x + y, u.curMax[i]) : fmax(y - x, 0)));
        u.curInc[i] = (u.curInc[i] && newVal >= u.curMax[i]) ? false :
                    (!u.curInc[i] && newVal <= 0)   ? true  : u.curInc[i];


        x = specRanNumGgen::getrandom(100);
        double &yy = u.powerFactor[i];
        newVal = u.pfInc[i] ? x+yy : yy-x;

        yy = ((u.pfInc[i] ? fmin(x + yy, 1) : fmax(yy - x, 0.2)));
        u.pfInc[i] = (u.pfInc[i] && newVal >= 1) ? false :
                          (!u.pfInc[i] && newVal <= 0.2)   ? true  : u.pfInc[i];

    }

    for(int i = 0; i < box.loopItemList.curValue.size(); i ++){
        u.powValue[i] = u.volValue[i] * u.curValue[i] * u.powerFactor[i] / 1000.0;
        u.powApparent[i] = u.volValue[i] * u.curValue[i] / 1000.0;

        u.powReactive[i] = sqrt(u.powApparent[i]*u.powApparent[i] - u.powValue[i]*u.powValue[i]);

        u.eleActive[i] += bus_sendTime*u.powValue[i] / 3600;
        u.eleReactive[i] += bus_sendTime*u.powReactive[i] / 3600;
    }

}

void MapProcessor::PRun(bool flag)
{
    m_running = flag;
}
