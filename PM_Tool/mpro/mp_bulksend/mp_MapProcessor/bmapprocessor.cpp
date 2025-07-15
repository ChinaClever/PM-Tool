#include "bmapprocessor.h"
#include "mp_bulksend.h"
#include "mp_tojson.h"

#include <QDebug>
#include <QQueue>

BMapProcessor::BMapProcessor(QObject* parent)
    :QThread(parent)
{
    qDebug()<<"BMapProcessor create";
    dbWriteThread = new DbWriteThread(this);
    //dbWriteThread->start();

    dbWriteTimer = new QTimer(this);
    dbWriteTimer->setInterval(33 * 60 * 1000); // 30分钟写一次
    //dbWriteTimer->setInterval(10000); // 30分钟写一次

    connect(dbWriteTimer, &QTimer::timeout, this, &BMapProcessor::writeDbTimeout);
    //dbWriteTimer->start();
}

BMapProcessor::~BMapProcessor()
{

    m_running = false;
    quit();
    wait();

    if (dbWriteTimer) {
        dbWriteTimer->stop();
        delete dbWriteTimer;
        dbWriteTimer = nullptr;
    }

    if (dbWriteThread) {
        dbWriteThread->requestInterruption();
        dbWriteThread->quit();
        dbWriteThread->wait();
        delete dbWriteThread;
        dbWriteThread = nullptr;
    }
}

void BMapProcessor::run()
{

    auto u = std::make_unique<mp_toJson>(nullptr);

    while(m_running){

        QDateTime t1 = QDateTime::currentDateTime();

        for(auto it = BMap.begin(); it != BMap.end(); ++it){
            auto &dev = it.value();
            envInc(dev);
            bitInc(dev);
            mp_bulksend::cirInti(dev);
            mp_bulksend::lineInti(dev);
            mp_bulksend::totalInti(dev);
            u->toJsonSlot(dev);

            auto json = u ->getJsonObject();
            {
                QMutexLocker locker(&ProBulkJQMutexes[1]);
                ProBulkJQs[1].enqueue(json);
            }
        }

        QDateTime t2 = QDateTime::currentDateTime();
        int duration = t1.msecsTo(t2);
        if(duration < mp_sendTime * 1000){
            msleep(mp_sendTime * 1000 - duration);
        }

    }

}

void BMapProcessor::BPRun(bool flag)
{
    m_running = flag;
}

void BMapProcessor::writeDbTimeout()
{
    QMutexLocker locker(&mapMutex);

    for (auto it = BMap.begin(); it != BMap.end(); ++it) {
        auto& dev = it.value();

        DbWriteTask task;
        task.table = DbWriteTask::OutputBit;
        task.key = dev.dev_key;


        int size = dev.pduData.outputData.outputBits.size();
        //qDebug()<<task.key ;
        for (int i = 0; i < size; ++i) {
            task.values.append(dev.pduData.outputData.outputBits[i].energy);
            //qDebug()<<dev.pduData.outputData.outputBits[i].energy<<' ';
        }
        for (int i = size; i < 48; ++i) {
            task.values.append(0.0);
        }


        dbWriteThread->enqueueTask(task);
    }

    qDebug() << "BMapProcessor 写数据库任务入队完成";
}
