#include "amapjsonqueue.h"
#include "mainpage_globals.h"

#include <QUdpSocket>
#include <mp_globals.h>
#include <QQueue>
#include <QJsonDocument>
AMapJsonQueue::AMapJsonQueue(QObject* parent)
{
    // qDebug()<<"AJsonQueue Processor create";
}
AMapJsonQueue::~AMapJsonQueue()
{
    m_running = false;
    wait();
    quit();
}
void AMapJsonQueue::run()
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
        while(!ProBulkJQs[0].isEmpty()) {

            {
                QMutexLocker locker(&ProBulkJQMutexes[0]);
                if(!ProBulkJQs[0].isEmpty()) u = ProBulkJQs[0].dequeue();
                //if(ProBulkJQs[0].size()>10000)
                  //  qDebug()<<" aa  "<<ProBulkJQs[0].size();
            }

            MpCnt++;
            MpCntt++;

            if (u.isEmpty()) {
                //qDebug() << "Empty JSON object, skipped";
                MpCntEr++;
                continue;
            }
            QByteArray jsonData = QJsonDocument(u).toJson(QJsonDocument::Compact);

            if(udpsocket->writeDatagram(jsonData, QHostAddress(ipAddress), Port) == -1) {
                //qWarning() << "Failed to send data:" << udpsocket->errorString();
                MpCntEr++;
                // 可以选择重试或记录错误
            }
            if((cnt++)%50 == 0)
                usleep(1);
        }
        msleep(1);
        if(MpCntt >= (Anum+Bnum+Cnum+Dnum)*0.95) {
            //qDebug()<<(Anum+Bnum+Cnum+Dnum)*0.95<<'?';
            MpCntt = 0;
            QDateTime t2 = QDateTime::currentDateTime();
            int duration = t1.msecsTo(t2);
            //emit TcheckTime(duration);
            // qDebug()<<duration;
        }

    }
}

void AMapJsonQueue::AJRun(bool flag)
{
    m_running = flag;
}
