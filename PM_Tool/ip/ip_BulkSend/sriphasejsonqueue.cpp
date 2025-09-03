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

    QString ipAddress2 = serIp2;   //需要停止发送才更新发送ip，三相暂时没有改
    QString portStr2 = port2;

 //   qDebug()<<ipAddress2<<portStr2;
    QString ipAddress3 = serIp3;   //需要停止发送才更新发送ip，三相暂时没有改
    QString portStr3 = port3;

    quint16 Port = portStr.toUShort();
    quint16 Port2 = portStr2.toUShort();
    quint16 Port3 = portStr3.toUShort();

    QJsonObject u;

    std::unique_ptr<QUdpSocket> udpsocket = std::make_unique<QUdpSocket>();
    int cnt = 0;
    while(m_running){

        cnt = 0;

        QDateTime t1 = QDateTime::currentDateTime();

        while(!SJsonQueue.isEmpty()) {

            {
                QMutexLocker locker(&SQueueMutex);
                if(!SJsonQueue.isEmpty()) u = SJsonQueue.dequeue();
                if(SJsonQueue.size()>10000)
                    qDebug()<<" aa  "<<SJsonQueue.size();
            }

            QByteArray jsonData = QJsonDocument(u).toJson(QJsonDocument::Compact);

            SCnt++;
            SCntt++;

            if(!ipAddress.isEmpty() && Port)
            if(udpsocket->writeDatagram(jsonData, QHostAddress(ipAddress), Port) == -1) {
                qWarning() << "Failed to send data:" << udpsocket->errorString();
                SCntEr++;
                // 可以选择重试或记录错误
            }

            if(!ipAddress2.isEmpty() && Port2)
            if(udpsocket->writeDatagram(jsonData, QHostAddress(ipAddress2), Port2) == -1) {
                qWarning() << "Failed to send data:" << udpsocket->errorString();
                SCntEr++;
                // 可以选择重试或记录错误
            }

            if(!ipAddress3.isEmpty() && Port3)
            if(udpsocket->writeDatagram(jsonData, QHostAddress(ipAddress3), Port3) == -1) {
                qWarning() << "Failed to send data:" << udpsocket->errorString();
                SCntEr++;
                // 可以选择重试或记录错误
            }


            if((cnt++)%50 == 0){
                // int s = QRandomGenerator::global()->bounded(1, 100);
                usleep(1);
            }
        }
        msleep(5);

        if(SCntt >= SsendNum*0.9) {
            SCntt = 0;
            QDateTime t2 = QDateTime::currentDateTime();
            int duration = t1.msecsTo(t2);
            emit ScheckTime(duration);
            //qDebug()<<duration;
        }

    }
}


void SriPhaseJsonQueue::sriRun(bool flag) //启动/关闭该线程
{
    if(flag)m_running = 1;
    else m_running = 0;
}

