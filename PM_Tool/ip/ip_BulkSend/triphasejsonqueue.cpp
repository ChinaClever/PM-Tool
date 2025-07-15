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



    QString ipAddress = serIp;
    QString portStr = port;
    quint16 Port = portStr.toUShort();
    std::unique_ptr<QUdpSocket> udpsocket = std::make_unique<QUdpSocket>();
    int cnt = 0;
    QJsonObject u;
    while(m_running){
        cnt = 0;
        QDateTime t1 = QDateTime::currentDateTime();
        while(!TJsonQueue.isEmpty()) {

            {
                QMutexLocker locker(&TQueueMutex);
                if(!TJsonQueue.isEmpty()) u = TJsonQueue.dequeue();
                if(TJsonQueue.size()>10000)
                    qDebug()<<" aa  "<<TJsonQueue.size();
            }

            QByteArray jsonData = QJsonDocument(u).toJson(QJsonDocument::Compact);

            TCnt++;
            TCntt++;
            if(udpsocket->writeDatagram(jsonData, QHostAddress(ipAddress), Port) == -1) {
                qWarning() << "Failed to send data:" << udpsocket->errorString();
                TCntEr++;
                // 可以选择重试或记录错误
            }
            if((cnt++)%50 == 0)
                usleep(1);
        }
        msleep(5);

        if(TCntt >= TsendNum*0.9) {
            TCntt = 0;
            QDateTime t2 = QDateTime::currentDateTime();
            int duration = t1.msecsTo(t2);
            emit TcheckTime(duration);
           // qDebug()<<duration;
        }
    }
}

void TriPhaseJsonQueue::triRun(bool flag)
{
    if(flag)m_running = 1;
    else m_running = 0;
}
