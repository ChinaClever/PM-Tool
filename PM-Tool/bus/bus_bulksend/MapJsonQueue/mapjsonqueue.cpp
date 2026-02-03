
#include "mapjsonqueue.h"
#include <QDebug>
#include <QUdpSocket>
#include <QJsonDocument>
#include <QRandomGenerator>
#include "bus_globals.h"
#include "mainpage_globals.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>
#include <QString>
#include <QDebug>
#include <cstring>
#include <QCoreApplication>

MapJsonQueue::MapJsonQueue(QObject* parent)
{

}

void MapJsonQueue::run()
{

    QJsonObject u;
    int cnt = 0;
    QString ipAddress = serIp;
    QString portStr = BusPort;

    QString ipAddress2 = serIp2;
    QString portStr2 = BusPort2;

    QString ipAddress3 = serIp3;
    QString portStr3 = BusPort3;

    QString MQTTipAddress = mqttserIp;
    QString MQTTportStr = mqttBusPort;


   // qDebug()<<BusPort;
    quint16 Port = portStr.toUShort();
    quint16 Port2 = portStr2.toUShort();
    quint16 Port3 = portStr3.toUShort();


    msleep(QRandomGenerator::global()->bounded(1, 1000));
    std::unique_ptr<QUdpSocket> udpsocket = std::make_unique<QUdpSocket>();
    udpsocket->setSocketOption(QAbstractSocket::SendBufferSizeSocketOption, 2 * 1024 * 1024);

    if (m_pahoClient == NULL) { // 检查是否已初始化
        QString uri = QString("tcp://%1:%2").arg(MQTTipAddress).arg(MQTTportStr);
        QString clientId = "MapQueuePublisher_Persistent"; // 使用一个持久的 Client ID

        int rc_init = MQTTClient_create(&m_pahoClient, uri.toUtf8().constData(),
                                        clientId.toUtf8().constData(),
                                        MQTTCLIENT_PERSISTENCE_NONE, nullptr);

        if(rc_init == MQTTCLIENT_SUCCESS) {
            MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
            conn_opts.keepAliveInterval = 60;
            conn_opts.cleansession = 0; // 建议设置为 0 以便在断开时保持会话状态

            rc_init = MQTTClient_connect(m_pahoClient, &conn_opts);
            if(rc_init != MQTTCLIENT_SUCCESS) {
                qDebug() << "MQTT Persistent connect FAILED, rc:" << rc_init;
                MQTTClient_destroy(&m_pahoClient);
                m_pahoClient = NULL;
            } else {
                qDebug() << "MQTT Persistent connection established to" << uri;
            }
        } else {
            qDebug() << "MQTT Persistent client create FAILED, rc:" << rc_init;
        }
    }
    while(m_running){

        cnt = 0;
        QDateTime t1 = QDateTime::currentDateTime();
       // qDebug()<<bulkBoxNum<<endl;
        while(!busQueue.isEmpty()) {

            {
                QMutexLocker locker(&busBulkJQMutexes);
                if(!busQueue.isEmpty()) u = busQueue.dequeue();
                if(busQueue.size()>20000){
                    busQueue.clear();
                     //qDebug()<<" aa  "<<busQueue.size();
                }

            }
            if (u.isEmpty()) {
                qDebug() << "Empty JSON object, skipped";
                BusCntEr++;
                continue;
            }
            QString routingKey = u.value("key").toString();
            QByteArray jsonData = QJsonDocument(u).toJson(QJsonDocument::Compact);

  //          if(sendMode == SendMode::UDP){
                BusCnt++;
                BusCntt++;

                if(!ipAddress.isEmpty() && Port) {
                    // 尝试发送
                    qint64 res = udpsocket->writeDatagram(jsonData, QHostAddress(ipAddress), Port);

                    if(res == -1) {
                        // 如果是缓冲区满，等待写入就绪后再试一次（仅限必要情况）
                        if(udpsocket->waitForBytesWritten(10)) {
                            res = udpsocket->writeDatagram(jsonData, QHostAddress(ipAddress), Port);
                        }
                    }

                    if(res == -1) {
                        qWarning() << "Still failed:" << udpsocket->errorString();
                        BusCntEr++;
                    }
                }


                if(!ipAddress2.isEmpty() && Port2)
                    if(udpsocket->writeDatagram(jsonData, QHostAddress(ipAddress2), Port2) == -1) {
                        qWarning() << "Failed to send2 data:" << udpsocket->errorString();
                        BusCntEr++;
                        // 可以选择重试或记录错误
                    }


                if(!ipAddress3.isEmpty() && Port3)
                    if(udpsocket->writeDatagram(jsonData, QHostAddress(ipAddress3), Port3) == -1) {
                        qWarning() << "Failed to send3 data:" << udpsocket->errorString();
                        BusCntEr++;
                        // 可以选择重试或记录错误
                    }

                int currentSize = busQueue.size();
                if (currentSize > 5000) {
                    if (cnt % 100 == 0) QThread::usleep(50); // 压力大，每100个包睡0.5ms
                } else {
                    if (cnt % 20 == 0) QThread::msleep(50);    // 压力小，每50个包睡1ms
                }
                cnt++;
 //           }
 //           else
            if(sendMode == SendMode::MQTT){
                if (m_pahoClient != NULL) {
                    // if (routingKey.isEmpty()) {
                    //     routingKey = "default";
                    // }
                    QString topic = QString("busMetaData");

                    int pub_rc = MQTTClient_publish(
                        m_pahoClient,
                        topic.toUtf8().constData(),
                        jsonData.size(),
                        jsonData.constData(),
                        0, // QoS 0
                        false,
                        nullptr
                        );

                    if (pub_rc != MQTTCLIENT_SUCCESS) {
                        qWarning() << "MQTT Publish failed to4" << topic << ", rc:" << pub_rc;
                        // TODO: 考虑增加错误计数或重试机制
                        BusCntEr++;
                    } else {
                        // 可选：增加 MQTT 成功发送计数
                    }
                } else {
                    qWarning() << "MQTT client is disconnected, data skipped.";
                    BusCntEr++;
                }
            }
            //sendMqtt(u,"",2);

        }
        msleep(10);

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
