#include "dmapjsonqueue.h"
#include "mp_tojson.h"
#include "mainpage_globals.h"
#include <QUdpSocket>

DMapJsonQueue::DMapJsonQueue(QObject *parent)
{
     //qDebug()<<"DJQ create";
}

DMapJsonQueue::~DMapJsonQueue()
{
    m_running = false;
    wait();
    quit();
}

void DMapJsonQueue::run()
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
    // 【D 模块 MQTT 连接初始化】
    // ------------------------------------------------------------------
    if (m_pahoClient == NULL) {
        QString uri = QString("tcp://%1:%2").arg(MQTTipAddress).arg(MQTTportStr);
        // 使用 D 模块专用的 Client ID
        QString clientId = "D_Publisher_Persistent";

        int rc_init = MQTTClient_create(&m_pahoClient, uri.toUtf8().constData(),
                                        clientId.toUtf8().constData(),
                                        MQTTCLIENT_PERSISTENCE_NONE, nullptr);

        if(rc_init == MQTTCLIENT_SUCCESS) {
            MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
            conn_opts.keepAliveInterval = 60;
            conn_opts.cleansession = 0; // 建议发布者使用非持久会话

            rc_init = MQTTClient_connect(m_pahoClient, &conn_opts);
            if(rc_init != MQTTCLIENT_SUCCESS) {
                qDebug() << "D MQTT Persistent connect FAILED, rc:" << rc_init;
                MQTTClient_destroy(&m_pahoClient);
                m_pahoClient = NULL;
            } else {
                qDebug() << "D MQTT Persistent connection established to" << uri;
            }
        } else {
            qDebug() << "D MQTT Persistent client create FAILED, rc:" << rc_init;
        }
    }
    // ------------------------------------------------------------------

    int cnt = 0;
    while(m_running){
        cnt = 0;
        QDateTime t1 = QDateTime::currentDateTime();

        if (sendMode == SendMode::MQTT && m_pahoClient == NULL) {
            QString uri = QString("tcp://%1:%2").arg(serIp).arg(Port);

            // 修复 2：使用新的、唯一的 Client ID
            QString timestamp = QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");
            QString clientId = QString("D_Publisher_Unique_%1").arg(timestamp);
            int rc_init = MQTTClient_create(&m_pahoClient, uri.toUtf8().constData(),
                                            clientId.toUtf8().constData(),
                                            MQTTCLIENT_PERSISTENCE_NONE, nullptr);

            if(rc_init == MQTTCLIENT_SUCCESS) {
                MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
                conn_opts.keepAliveInterval = 60;
                conn_opts.cleansession = 1; // 修复 2：使用 Clean Session (非持久会话)

                rc_init = MQTTClient_connect(m_pahoClient, &conn_opts);
                if(rc_init != MQTTCLIENT_SUCCESS) {
                    qDebug() << "D MQTT Persistent connect FAILED, rc:" << rc_init;
                    MQTTClient_destroy(&m_pahoClient);
                    m_pahoClient = NULL;
                } else {
                    qDebug() << "D MQTT Persistent connection established to" << uri;
                }
            } else {
                qDebug() << "D MQTT Persistent client create FAILED, rc:" << rc_init;
            }

            // 修复 1：如果连接失败，等待 500ms 后重试
            if (m_pahoClient == NULL) {
                continue; // 跳到下一个主循环，重新尝试连接
            }
        }

        // 队列读取 ProBulkJQs[3] (正确)
        while(!ProBulkJQs[3].isEmpty()){
            {
                QMutexLocker locker(&ProBulkJQMutexes[3]);
                if(!ProBulkJQs[3].isEmpty()) u = ProBulkJQs[3].dequeue();
            }

            // 【重要】将 MpCnt/MpCntt 移动到成功发送块中，保持与 A/B/C 的新逻辑一致

            if (u.isEmpty()) {
                MpCntEr++; // 假设 D 模块使用 MpCntEr 计数
                continue;
            }
            QByteArray jsonData = QJsonDocument(u).toJson(QJsonDocument::Compact);

            // --------------------------------------------------------
            // 【模式切换与发送逻辑】
            // --------------------------------------------------------


                // --- UDP 目标 1 ---
                if(!ipAddress.isEmpty() && Port) {
                    if(udpsocket->writeDatagram(jsonData, QHostAddress(ipAddress), Port) != -1) {
                    } else {
                        MpCntEr++;
                    }
                }
                // --- UDP 目标 2 ---
                if(!ipAddress2.isEmpty() && Port2) {
                    if(udpsocket->writeDatagram(jsonData, QHostAddress(ipAddress2), Port2) != -1) {
                    } else {
                        MpCntEr++;
                    }
                }
                // --- UDP 目标 3 ---
                if(!ipAddress3.isEmpty() && Port3) {
                    if(udpsocket->writeDatagram(jsonData, QHostAddress(ipAddress3), Port3) != -1) {
                    } else {
                        MpCntEr++;
                    }
                }
                int currentSize = ProBulkJQs[3].size();
                if (currentSize > 5000) {
                    if (cnt % 100 == 0) QThread::usleep(50); // 压力大，每100个包睡0.5ms
                } else {
                    if (cnt % 20 == 0) QThread::msleep(50);    // 压力小，每50个包睡1ms
                }
            if (sendMode == SendMode::MQTT) {

                // 【MQTT 逻辑：持久连接 + 重连 + 成功计数】
                if (m_pahoClient != NULL) {

                    // 1. 自动重连检查
                    if (!MQTTClient_isConnected(m_pahoClient)) {
                        MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
                        conn_opts.keepAliveInterval = 60;
                        conn_opts.cleansession = 1;

                        qWarning() << "D MQTT Disconnected. Attempting to reconnect...";
                        int conn_status = MQTTClient_connect(m_pahoClient, &conn_opts);

                        if (conn_status != MQTTCLIENT_SUCCESS) {
                            qWarning() << "D MQTT Reconnect FAILED, rc:" << conn_status;
                            MpCntEr++;
                            continue; // 重连失败，跳过本次发送
                        } else {
                            qDebug() << "D MQTT Reconnected successfully.";
                        }
                    }

                    // 2. 发布消息
                    QString topic = QString("pduMetaData"); // D 模块专用的 Topic

                    int pub_rc = MQTTClient_publish(
                        m_pahoClient, topic.toUtf8().constData(),
                        jsonData.size(), jsonData.constData(),
                        0, false, nullptr
                        );

                    if (pub_rc != MQTTCLIENT_SUCCESS) {
                        qWarning() << "D MQTT Publish failed to" << topic << ", rc:" << pub_rc;
                        MpCntEr++;
                    } else {
                        // 3. 成功发送时，增加计数
                        MpCnt++;
                        MpCntt++;
                    }
                } else {
                    qWarning() << "D MQTT client not initialized/disconnected, data skipped.";
                    MpCntEr++;
                }
            } // 结束 MQTT 模式
            // --------------------------------------------------------
        }
        usleep(10);

        // 计时逻辑 (保持不变)
        if(MpCntt >= (Anum+Bnum+Cnum+Dnum)*0.9) {
            MpCntt = 0;
            QDateTime t2 = QDateTime::currentDateTime();
            int duration = t1.msecsTo(t2);
            //emit TcheckTime(duration);
        }
    }
}
void DMapJsonQueue::DJRun(bool flag)
{
    m_running = flag;
}
