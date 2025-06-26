#include "mapjsonqueue.h"
#include <QDebug>
#include <QUdpSocket>
#include <QJsonDocument>
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
    QString portStr = port;
    quint16 Port = portStr.toUShort();
    std::unique_ptr<QUdpSocket> udpsocket = std::make_unique<QUdpSocket>();
    while(m_running){

        cnt = 0;
        while(!busQueue.isEmpty()) {

            {
                QMutexLocker locker(&busBulkJQMutexes);
                if(!busQueue.isEmpty()) u = busQueue.dequeue();
                if(busQueue.size()>10000)
                    qDebug()<<" aa  "<<busQueue.size();
            }

            if (u.isEmpty()) {
                qDebug() << "Empty JSON object, skipped";
                continue;
            }
            QByteArray jsonData = QJsonDocument(u).toJson(QJsonDocument::Compact);

            if(udpsocket->writeDatagram(jsonData, QHostAddress(ipAddress), Port) == -1) {
                qWarning() << "Failed to send data:" << udpsocket->errorString();
                // 可以选择重试或记录错误
            }
            if((cnt++)%10 == 0)
                usleep(1);
        }
        msleep(1);
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
