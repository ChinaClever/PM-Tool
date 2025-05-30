#include "sriphasejsonqueue.h"
#include <QDebug>
#include "globals.h"
#include <QUdpSocket>
#include "data_cal/data_cal.h"
#include <memory>
#include "mainpage_globals.h"
SriPhaseJsonQueue::SriPhaseJsonQueue(QObject* parent)
{
   // qDebug() << "Squeue Processor created";
}
SriPhaseJsonQueue::~SriPhaseJsonQueue()
{
    m_running = false;
    wait();
    quit();

}
void SriPhaseJsonQueue::run()
{


    QString ipAddress = serIp;   //需要停止发送才更新发送ip，三相暂时没有改
    QString portStr = port;
    quint16 Port = portStr.toUShort();
    QJsonObject u;

    std::unique_ptr<QUdpSocket> udpsocket = std::make_unique<QUdpSocket>();
    int cnt = 0;
    while(m_running){

        cnt = 0;

        while(!SJsonQueue.isEmpty()) {

            SQueueMutex.lock();
            if(!SJsonQueue.isEmpty()) u = SJsonQueue.dequeue();
            if(SJsonQueue.size()>10000)
            qDebug()<<" aa  "<<SJsonQueue.size();
            SQueueMutex.unlock();


            QByteArray jsonData = QJsonDocument(u).toJson(QJsonDocument::Compact);


            if(udpsocket->writeDatagram(jsonData, QHostAddress(ipAddress), Port) == -1) {
                qWarning() << "Failed to send data:" << udpsocket->errorString();
                // 可以选择重试或记录错误
            }
            if((cnt++)%500 == 0)
                usleep(1);
        }
        QThread::usleep(100);

    }
}


void SriPhaseJsonQueue::sriRun(bool flag) //启动/关闭该线程
{
    if(flag)m_running = 1;
    else m_running = 0;
}

