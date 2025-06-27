#include "dmapprocessor.h"
#include "mp_bulksend.h"
#include "mp_tojson.h"
#include <QDebug>
#include <QQueue>

DMapProcessor::DMapProcessor(QObject* parent)
    :QThread(parent)
{
    qDebug()<<"DMapProcessor create";
}

DMapProcessor::~DMapProcessor()
{

    m_running = false;
    wait();
    quit();
}

void DMapProcessor::run()
{

    auto u = std::make_unique<mp_toJson>(nullptr);

    while(m_running){

        QDateTime t1 = QDateTime::currentDateTime();

        for(auto it = DMap.begin(); it != DMap.end(); ++it){
            auto &dev = it.value();
            envInc(dev);
            bitInc(dev);
            mp_bulksend::cirInti(dev);
            mp_bulksend::lineInti(dev);
            mp_bulksend::totalInti(dev);
            u->toJsonSlot(dev);

            auto json = u ->getJsonObject();
            {
                QMutexLocker locker(&ProBulkJQMutexes[3]);
                ProBulkJQs[3].enqueue(json);
            }
        }

        QDateTime t2 = QDateTime::currentDateTime();
        int duration = t1.msecsTo(t2);
        if(duration < mp_sendTime * 1000){
            msleep(mp_sendTime * 1000 - duration);
        }

    }

}

void DMapProcessor::DPRun(bool flag)
{
    m_running = flag;
}
