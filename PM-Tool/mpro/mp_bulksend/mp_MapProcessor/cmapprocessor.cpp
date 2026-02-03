#include "cmapprocessor.h"
#include "mp_bulksend.h"
#include "mp_tojson.h"

#include <QDebug>
#include <QQueue>

CMapProcessor::CMapProcessor(QObject* parent)
    :QThread(parent)
{
     //qDebug()<<"CMapProcessor create";
    dbWriteThread = new DbWriteThread(this);
    dbWriteThread->start();

    // 定时器写数据库，每30分钟写一次，视需求可改
    dbWriteTimer = new QTimer(this);
    dbWriteTimer->setInterval(34 * 60 * 1000);
    connect(dbWriteTimer, &QTimer::timeout, this, &CMapProcessor::writeDbTimeout);
    dbWriteTimer->start();
}

CMapProcessor::~CMapProcessor()
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

void CMapProcessor::writeDbTimeout()
{
    QMutexLocker locker(&mapMutex);

    for (auto it = CMap.begin(); it != CMap.end(); ++it) {
        auto& dev = it.value();

        DbWriteTask task;
        task.table = DbWriteTask::OutputBit;  // 根据你表类型修改
        task.key = dev.dev_key;

        int size = dev.pduData.outputData.outputBits.size();

        // 添加实际电能数据
        for (int i = 0; i < size; ++i) {
            task.values.append(dev.pduData.outputData.outputBits[i].energy);
        }

        // 补齐到48个，数据库写入函数要求
        for (int i = size; i < 48; ++i) {
            task.values.append(0.0);
        }

        dbWriteThread->enqueueTask(task);
    }

    qDebug() << "CMapProcessor 写数据库任务入队完成";
}
