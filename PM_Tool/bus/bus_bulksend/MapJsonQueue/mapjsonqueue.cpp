
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

MapJsonQueue::MapJsonQueue(QObject* parent)
{

}

// 保持函数签名不变，但内部忽略 brokerIp 和 brokerPort 参数
bool MapJsonQueue::sendMqtt(const QJsonObject &obj, const QString &brokerIp, quint16 brokerPort) {

    // ------------------------------------------------------------------
    // 【核心修改】硬编码 Broker 地址和端口，忽略传入参数
    // ------------------------------------------------------------------
    const char* fixedBrokerIp = "broker.mqtt.cool";
    quint16 fixedBrokerPort = 1883;

    // 1. 提取 Client ID
    QString clientId = obj.value("key").toString();
    if(clientId.isEmpty()) {
        clientId = "qt_default";
    }

    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

    // 2. 构建连接 URI (使用固定地址和端口，并修正格式)
    // 格式为标准的 TCP 连接格式: tcp://IP:Port
    QString uri = QString("tcp://%1:%2").arg(fixedBrokerIp).arg(fixedBrokerPort);

    // --- 客户端创建 ---
    int rc = MQTTClient_create(&client, uri.toUtf8().constData(),
                               clientId.toUtf8().constData(),
                               MQTTCLIENT_PERSISTENCE_NONE, nullptr);
    if(rc != MQTTCLIENT_SUCCESS) {
        qDebug() << "MQTT create failed, rc:" << rc;
        return false;
    }

    // --- 连接配置与连接 ---
    conn_opts.keepAliveInterval = 60;
    conn_opts.cleansession = 1;

    rc = MQTTClient_connect(client, &conn_opts);
    if(rc != MQTTCLIENT_SUCCESS) {
        qDebug() << "MQTT connect failed, rc:" << rc;
        MQTTClient_destroy(&client);
        return false;
    }

    // --- 发布消息 ---
    QByteArray data = QJsonDocument(obj).toJson(QJsonDocument::Compact);

    // QoS = 0，固定主题 "qt/test"
    rc = MQTTClient_publish(client, "qt/test", data.size(), data.constData(), 0, false, nullptr);
    qDebug() << "MQTT publish rc:" << rc;

    // --- 清理 ---
    MQTTClient_disconnect(client, 1000);
    MQTTClient_destroy(&client);

    return rc == MQTTCLIENT_SUCCESS;
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


   // qDebug()<<BusPort;
    quint16 Port = portStr.toUShort();
    quint16 Port2 = portStr2.toUShort();
    quint16 Port3 = portStr3.toUShort();


    msleep(QRandomGenerator::global()->bounded(1, 1000));
    std::unique_ptr<QUdpSocket> udpsocket = std::make_unique<QUdpSocket>();
    const char* FIXED_BROKER_IP = "broker.mqtt.cool";
    const quint16 FIXED_BROKER_PORT = 1883;
    if (m_pahoClient == NULL) { // 检查是否已初始化
        QString uri = QString("tcp://%1:%2").arg(FIXED_BROKER_IP).arg(FIXED_BROKER_PORT);
        QString clientId = "MapQueuePublisher_Persistent"; // 使用一个持久的 Client ID

        int rc_init = MQTTClient_create(&m_pahoClient, uri.toUtf8().constData(),
                                        clientId.toUtf8().constData(),
                                        MQTTCLIENT_PERSISTENCE_NONE, nullptr);

        if(rc_init == MQTTCLIENT_SUCCESS) {
            MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
            conn_opts.keepAliveInterval = 60;
            conn_opts.cleansession = 1; // 建议设置为 0 以便在断开时保持会话状态

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
                if(busQueue.size()>10)
                    qDebug()<<" aa  "<<busQueue.size();
            }
            if (u.isEmpty()) {
                qDebug() << "Empty JSON object, skipped";
                BusCntEr++;
                continue;
            }
            QString routingKey = u.value("key").toString();
            QByteArray jsonData = QJsonDocument(u).toJson(QJsonDocument::Compact);

            // if(udp){
            //     BusCnt++;
            //     BusCntt++;


            //     if(!ipAddress.isEmpty() && Port)
            //         if(udpsocket->writeDatagram(jsonData, QHostAddress(ipAddress), Port) == -1) {
            //             qWarning() << "Failed to send data:" << udpsocket->errorString();
            //             BusCntEr++;
            //             // 可以选择重试或记录错误
            //         }


            //     if(!ipAddress2.isEmpty() && Port2)
            //         if(udpsocket->writeDatagram(jsonData, QHostAddress(ipAddress2), Port2) == -1) {
            //             qWarning() << "Failed to send data:" << udpsocket->errorString();
            //             BusCntEr++;
            //             // 可以选择重试或记录错误
            //         }


            //     if(!ipAddress3.isEmpty() && Port3)
            //         if(udpsocket->writeDatagram(jsonData, QHostAddress(ipAddress3), Port3) == -1) {
            //             qWarning() << "Failed to send data:" << udpsocket->errorString();
            //             BusCntEr++;
            //             // 可以选择重试或记录错误
            //         }

            //     if((cnt++)%50 == 0){
            //         // int s = QRandomGenerator::global()->bounded(1, 100);
            //         usleep(1);
            //     }
            // }
            // else if(mqtt){

            // }
            //sendMqtt(u,"",2);
            if (m_pahoClient != NULL) {
                if (routingKey.isEmpty()) {
                    routingKey = "default";
                }

                QString topic = QString("qt/test");

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
                    qWarning() << "MQTT Publish failed to" << topic << ", rc:" << pub_rc;
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
