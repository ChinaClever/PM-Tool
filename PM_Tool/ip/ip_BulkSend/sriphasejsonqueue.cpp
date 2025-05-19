#include "sriphasejsonqueue.h"
#include <QDebug>
#include "globals.h"
#include <QUdpSocket>
#include "data_cal/data_cal.h"
SriPhaseJsonQueue::SriPhaseJsonQueue(QObject* parent)
{
    qDebug() << "Squeue Processor created";
}
SriPhaseJsonQueue::~SriPhaseJsonQueue()
{
    m_running = false;
    wait();
    quit();

}
void SriPhaseJsonQueue::run()
{

    QQueue<QJsonObject>arr;
    while(m_running){

        {
            QMutexLocker locker(&SQueueMutex);
            arr = SJsonQueue;
            SJsonQueue.clear();
        }
        QUdpSocket *udpsocket = new QUdpSocket();

        //判断发送起始时间和结束时间，确定sleep时间
        QString x = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

        while(!arr.isEmpty()) {
            auto u = arr.dequeue();
            QByteArray jsonData = QJsonDocument(u).toJson(QJsonDocument::Compact);

            if(udpsocket->writeDatagram(jsonData, QHostAddress("192.168.1.246"), 6002) == -1) {
                qWarning() << "Failed to send data:" << udpsocket->errorString();
                // 可以选择重试或记录错误
            }
            msleep(1);
        }

        QString y = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        int k = data_cal::calculateTimeDiffInSeconds(y,x);
        if(k<=Stimesend)
        sleep(Stimesend-k);
    }
}



void SriPhaseJsonQueue::sriRun(bool flag) //启动/关闭该线程
{
    if(flag)m_running = 1;
    else m_running = 0;
}

