
#include "mapjsonqueue.h"
#include <QDebug>
#include <QUdpSocket>
#include <QJsonDocument>
#include <QRandomGenerator>
#include "bus_globals.h"
#include "mainpage_globals.h"
MapJsonQueue::MapJsonQueue(QObject* parent)
{

}

void MapJsonQueue::run()
{

    QJsonObject u;
    int cnt = 0;
    QString ipAddress = serIp;
    QString portStr = BusPort;
   // qDebug()<<BusPort;
    quint16 Port = portStr.toUShort();
    msleep(QRandomGenerator::global()->bounded(1, 1000));
    std::unique_ptr<QUdpSocket> udpsocket = std::make_unique<QUdpSocket>();
    while(m_running){

        cnt = 0;
        QDateTime t1 = QDateTime::currentDateTime();
       // qDebug()<<bulkBoxNum<<endl;
        while(!busQueue.isEmpty()) {

            {
                QMutexLocker locker(&busBulkJQMutexes);
                if(!busQueue.isEmpty()) u = busQueue.dequeue();
                if(busQueue.size()>10000)
                    qDebug()<<" aa  "<<busQueue.size();
            }
            if (u.isEmpty()) {
                qDebug() << "Empty JSON object, skipped";
                BusCntEr++;
                continue;
            }
            QByteArray jsonData = QJsonDocument(u).toJson(QJsonDocument::Compact);

            BusCnt++;
            BusCntt++;
            if(udpsocket->writeDatagram(jsonData, QHostAddress(ipAddress), Port) == -1) {
                qWarning() << "Failed to send data:" << udpsocket->errorString();
                BusCntEr++;
                // 可以选择重试或记录错误
            }
            if((cnt++)%50 == 0){
             // int s = QRandomGenerator::global()->bounded(1, 100);
                usleep(1);
            }
        }
        msleep(5);

        //qDebug()<<BusCntt<<' '<<(bulkBoxNum+1)*bulkBusNum;
        if(BusCntt >= bulkBoxNum*bulkBusNum) {

            BusCntt = 0;
            QDateTime t2 = QDateTime::currentDateTime();
            int duration = t1.msecsTo(t2);
            emit checkTime(duration);
          //  qDebug()<<duration;
        }
    }

}

MapJsonQueue::~MapJsonQueue()
{
    m_running = false;
    wait();
    quit();
}

void MapJsonQueue::JRun(bool flag)
{
    m_running = flag;
}
