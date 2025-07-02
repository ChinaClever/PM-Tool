#include "cmapprocessor.h"
#include "mp_bulksend.h"
#include "mp_tojson.h"

#include <QDebug>
#include <QQueue>

CMapProcessor::CMapProcessor(QObject* parent)
    :QThread(parent)
{
    qDebug()<<"CMapProcessor create";
}

CMapProcessor::~CMapProcessor()
{
    m_running = false;
    wait();
    quit();
}

void CMapProcessor::run()
{

    auto u = std::make_unique<mp_toJson>(nullptr);

    while(m_running){

        QDateTime t1 = QDateTime::currentDateTime();

        for(auto it = CMap.begin(); it != CMap.end(); ++it){
            auto &dev = it.value();
            envInc(dev);
            bitInc(dev);
            mp_bulksend::cirInti(dev);
            mp_bulksend::lineInti(dev);
            mp_bulksend::totalInti(dev);
            u->toJsonSlot(dev);

            auto json = u ->getJsonObject();
            {
                QMutexLocker locker(&ProBulkJQMutexes[2]);
                ProBulkJQs[2].enqueue(json);
            }
        }

        QDateTime t2 = QDateTime::currentDateTime();
        int duration = t1.msecsTo(t2);
        if(duration < mp_sendTime * 1000){
            msleep(mp_sendTime * 1000 - duration);
        }

    }

}

void CMapProcessor::CPRun(bool flag)
{
    m_running = flag;
}
