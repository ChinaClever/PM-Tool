#include "bmapjsonqueue.h"
#include "mp_tojson.h"
#include "mainpage_globals.h"
#include <QUdpSocket>

BMapJsonQueue::BMapJsonQueue(QObject *parent)
{
    qDebug()<<"BJQ create";
}

BMapJsonQueue::~BMapJsonQueue()
{
    m_running = false;
    wait();
    quit();
}

void BMapJsonQueue::run()
{
    QString ipAddress = serIp;
    QString portStr = port;
    quint16 Port = portStr.toUShort();
    QJsonObject u;

    std::unique_ptr<QUdpSocket> udpsocket = std::make_unique<QUdpSocket>();
    int cnt = 0;
    while(m_running){

        cnt = 0;
        QDateTime t1 = QDateTime::currentDateTime();

        while(!ProBulkJQs[1].isEmpty()){
            {
                QMutexLocker locker(&ProBulkJQMutexes[1]);
                if(!ProBulkJQs[1].isEmpty()) u = ProBulkJQs[1].dequeue();
               // if(ProBulkJQs[1].size()>10000)
                   // qDebug()<<" bb  "<<ProBulkJQs[1].size();
            }

            MpCnt++;
            MpCntt++;

            if (u.isEmpty()) {
                qDebug() << "Empty data packet, skipped";
                MpCntEr++;
                continue;
            }
            QByteArray jsonData = QJsonDocument(u).toJson(QJsonDocument::Compact);
            if(udpsocket->writeDatagram(jsonData, QHostAddress(ipAddress), Port) == -1) {
                qWarning() << "Failed to send data:" << udpsocket->errorString();
                MpCntEr++;
                // 可以选择重试或记录错误
            }
            if((cnt++)%50 == 0)
                usleep(1);
        }
        usleep(10);
        if(MpCntt >= (Anum+Bnum+Cnum+Dnum)*0.9) {
            MpCntt = 0;
            QDateTime t2 = QDateTime::currentDateTime();
            int duration = t1.msecsTo(t2);
            //emit TcheckTime(duration);
            // qDebug()<<duration;
        }
    }

}

void BMapJsonQueue::BJRun(bool flag)
{
    m_running = flag;
}
