#include "amapprocessor.h"
#include "mp_globals.h"
#include "mp_bulksend.h"
#include "mp_tojson.h"
#include <QDebug>
#include <QQueue>
#include <QJsonObject>

AMapProcessor::AMapProcessor(QObject* parent)
    :QThread(parent)
{
    qDebug()<<"AMapProcessor creater";
}
AMapProcessor::~AMapProcessor()
{

    m_running = false;
    wait();
    quit();
}

void AMapProcessor::run()
{
    auto u = std::make_unique<mp_toJson>(nullptr);
    while(m_running){

        QDateTime t1 = QDateTime::currentDateTime();
        for(auto it = AMap.begin(); it != AMap.end(); it++){

            envInc(it.value());
            bitInc(it.value());
            mp_bulksend::cirInti(it.value());
            mp_bulksend::lineInti(it.value());
            mp_bulksend::totalInti(it.value());

            u->toJsonSlot(it.value());
            QJsonObject json = u->getJsonObject();

            {
                QMutexLocker locker(&ProBulkJQMutexes[0]);
                ProBulkJQs[0].enqueue(json);
            }
        }

        //qDebug()<<ProBulkJQs.size();

        QDateTime t2 = QDateTime::currentDateTime();
        int duration = t1.msecsTo(t2);

        qDebug()<<"duration: "<<duration;
        if(duration<=mp_sendTime*1000)
            msleep(mp_sendTime*1000-duration);

    }

}

void AMapProcessor::APRun(bool flag)
{
    m_running = flag;
}
