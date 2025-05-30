#include "triphasejsonqueue.h"
#include <QDebug>
#include "globals.h"
#include <QUdpSocket>
#include "data_cal/data_cal.h"
#include "mainpage_globals.h"
TriPhaseJsonQueue::TriPhaseJsonQueue(QObject* parent)
    : QThread(parent)
{
   // qDebug() << "Tqueue Processor created";


}
TriPhaseJsonQueue::~TriPhaseJsonQueue()
{
    m_running = false;
    wait();
    quit();

}

void TriPhaseJsonQueue::run()
{
    QQueue<QJsonObject>arr;



    std::unique_ptr<QUdpSocket> udpsocket = std::make_unique<QUdpSocket>();
    int cnt = 0;
    while(m_running){
        {
            QMutexLocker locker(&TQueueMutex);
            arr = TJsonQueue;
           // qDebug()<<" aa  "<<TJsonQueue.size();
            TJsonQueue.clear();
        }
        cnt = 0;
        QString ipAddress = serIp;
        QString portStr = port;
        quint16 Port = portStr.toUShort();

       // qDebug()<<Port<<" "<<ipAddress;
        while(!arr.isEmpty()) {
            auto u = arr.dequeue();
            QByteArray jsonData = QJsonDocument(u).toJson(QJsonDocument::Compact);

            if(udpsocket->writeDatagram(jsonData, QHostAddress(ipAddress), Port) == -1) {
                qWarning() << "Failed to send data:" << udpsocket->errorString();
                // 可以选择重试或记录错误
            }
            if((cnt++)%100 == 0)
                usleep(1);
        }
        msleep(1);

    }
}

void TriPhaseJsonQueue::triRun(bool flag)
{
    if(flag)m_running = 1;
    else m_running = 0;
}
