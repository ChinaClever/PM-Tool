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
// 确保您已在 SriPhaseJsonQueue.h 中包含 <MQTTClient.h> 并声明 MQTTClient m_pahoClient = NULL;

void SriPhaseJsonQueue::run()
{
    // 配置信息（假设 ipAddress/Port 是 MQTT Broker 和 UDP 目标 1 的地址）
    QString ipAddress = serIp;
    QString portStr = port;
    quint16 Port = portStr.toUShort();

    QString ipAddress2 = serIp2;
    QString portStr2 = port2;
    quint16 Port2 = portStr2.toUShort();

    QString ipAddress3 = serIp3;
    QString portStr3 = port3;
    quint16 Port3 = portStr3.toUShort();

    QString MQTTipAddress = mqttserIp;
    QString MQTTportStr = mqttport;

    QJsonObject u;
    std::unique_ptr<QUdpSocket> udpsocket = std::make_unique<QUdpSocket>();

    // ------------------------------------------------------------------
    // 【三相模块 MQTT 连接初始化】
    // ------------------------------------------------------------------
    if (m_pahoClient == NULL) {
        QString uri = QString("tcp://%1:%2").arg(MQTTipAddress).arg(MQTTportStr);
        // 使用三相模块专用的 Client ID
        QString clientId = "SriPhase_Publisher_Persistent";

        int rc_init = MQTTClient_create(&m_pahoClient, uri.toUtf8().constData(),
                                        clientId.toUtf8().constData(),
                                        MQTTCLIENT_PERSISTENCE_NONE, nullptr);

        if(rc_init == MQTTCLIENT_SUCCESS) {
            MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
            conn_opts.keepAliveInterval = 60;
            conn_opts.cleansession = 1; // 建议发布者使用非持久会话

            rc_init = MQTTClient_connect(m_pahoClient, &conn_opts);
            if(rc_init != MQTTCLIENT_SUCCESS) {
                qDebug() << "SriPhase MQTT Persistent connect FAILED, rc:" << rc_init;
                MQTTClient_destroy(&m_pahoClient);
                m_pahoClient = NULL;
            } else {
                qDebug() << "SriPhase MQTT Persistent connection established to" << uri;
            }
        } else {
            qDebug() << "SriPhase MQTT Persistent client create FAILED, rc:" << rc_init;
        }
    }
    // ------------------------------------------------------------------

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

            if (u.isEmpty()) {
                SCntEr++;
                continue;
            }

            QByteArray jsonData = QJsonDocument(u).toJson(QJsonDocument::Compact);

            // --------------------------------------------------------
            // 【模式切换与发送逻辑】
            // --------------------------------------------------------

            // // 假设 sendModeSri 是全局控制三相模块发送模式的变量
            // if(sendMode == SendMode::UDP) {

                bool sent_ok = false;

                // --- UDP 目标 1 ---
                if(!ipAddress.isEmpty() && Port) {
                    if(udpsocket->writeDatagram(jsonData, QHostAddress(ipAddress), Port) != -1) {
                        sent_ok = true;
                    } else {
                        qWarning() << "Failed to send data (1):" << udpsocket->errorString();
                        SCntEr++;
                    }
                }
                // --- UDP 目标 2 ---
                if(!ipAddress2.isEmpty() && Port2) {
                    if(udpsocket->writeDatagram(jsonData, QHostAddress(ipAddress2), Port2) != -1) {
                        sent_ok = true;
                    } else {
                        qWarning() << "Failed to send data (2):" << udpsocket->errorString();
                        SCntEr++;
                    }
                }
                // --- UDP 目标 3 ---
                if(!ipAddress3.isEmpty() && Port3) {
                    if(udpsocket->writeDatagram(jsonData, QHostAddress(ipAddress3), Port3) != -1) {
                        sent_ok = true;
                    } else {
                        qWarning() << "Failed to send data (3):" << udpsocket->errorString();
                        SCntEr++;
                    }
                }

                // 统一成功计数：只要有一个目标成功，就计为成功发送
                if (sent_ok) {
                    SCnt++;
                    SCntt++;
                }

//            } else
                if (sendMode == SendMode::MQTT) {

                // 【MQTT 逻辑：持久连接 + 重连 + 成功计数】
                if (m_pahoClient != NULL) {

                    // 1. 自动重连检查
                    if (!MQTTClient_isConnected(m_pahoClient)) {
                        MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
                        conn_opts.keepAliveInterval = 60;
                        conn_opts.cleansession = 1;

                        qWarning() << "SriPhase MQTT Disconnected. Attempting to reconnect...";
                        int conn_status = MQTTClient_connect(m_pahoClient, &conn_opts);

                        if (conn_status != MQTTCLIENT_SUCCESS) {
                            qWarning() << "SriPhase MQTT Reconnect FAILED, rc:" << conn_status;
                            SCntEr++;
                            continue; // 重连失败，跳过本次发送
                        } else {
                            qDebug() << "SriPhase MQTT Reconnected successfully.";
                        }
                    }

                    // 2. 发布消息
                    QString topic = QString("pduMetaData"); // 三相模块专用的 Topic

                    int pub_rc = MQTTClient_publish(
                        m_pahoClient, topic.toUtf8().constData(),
                        jsonData.size(), jsonData.constData(),
                        0, false, nullptr
                        );

                    if (pub_rc != MQTTCLIENT_SUCCESS) {
                        qWarning() << "SriPhase MQTT Publish failed to" << topic << ", rc:" << pub_rc;
                        SCntEr++;
                    } else {
                        // 3. 成功发送时，增加计数
                        SCnt++;
                        SCntt++;
                    }
                } else {
                    qWarning() << "SriPhase MQTT client not initialized/disconnected, data skipped.";
                    SCntEr++;
                }
            } // 结束 MQTT 模式
            // --------------------------------------------------------

            if((cnt++)%50 == 0){
                usleep(1);
            }
        }
        msleep(5);

        // 计时逻辑 (保持不变)
        if(SCntt >= SsendNum*0.9) {
            SCntt = 0;
            QDateTime t2 = QDateTime::currentDateTime();
            int duration = t1.msecsTo(t2);
            emit ScheckTime(duration);
        }
    }
}

void SriPhaseJsonQueue::sriRun(bool flag) //启动/关闭该线程
{
    if(flag)m_running = 1;
    else m_running = 0;
}

