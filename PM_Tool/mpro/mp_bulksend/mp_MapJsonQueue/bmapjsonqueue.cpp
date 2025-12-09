#include "bmapjsonqueue.h"
#include "mp_tojson.h"
#include "mainpage_globals.h"
#include <QUdpSocket>
#include <QDateTime> // 引入 QDateTime 用于生成唯一ID

BMapJsonQueue::BMapJsonQueue(QObject *parent)
{
    // 确保 m_pahoClient 在类成员中初始化为 NULL
}

BMapJsonQueue::~BMapJsonQueue()
{
    m_running = false;
    // 退出前断开 MQTT 连接并销毁客户端
    if (m_pahoClient != NULL) {
        MQTTClient_disconnect(m_pahoClient, 1000);
        MQTTClient_destroy(&m_pahoClient);
        m_pahoClient = NULL;
    }
    wait();
    quit();
}

void BMapJsonQueue::run()
{
    // 配置信息
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

    int cnt = 0;
    while(m_running){

        // ------------------------------------------------------------------
        // 【第一层：连接初始化和重试逻辑】
        // 仅在客户端不存在 (NULL) 时执行昂贵的 create/connect 操作
        // ------------------------------------------------------------------
        if (sendMode == SendMode::MQTT && m_pahoClient == NULL) {
            QString uri = QString("tcp://%1:%2").arg(MQTTipAddress).arg(MQTTportStr);

            // 优化：使用动态 Client ID，防止冲突
            QString timestamp = QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");
            QString clientId = QString("B_Publisher_Unique_%1").arg(timestamp);

            int rc_init = MQTTClient_create(&m_pahoClient, uri.toUtf8().constData(),
                                            clientId.toUtf8().constData(),
                                            MQTTCLIENT_PERSISTENCE_NONE, nullptr);

            if(rc_init == MQTTCLIENT_SUCCESS) {
                MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
                conn_opts.keepAliveInterval = 60;
                conn_opts.cleansession = 1; // 确保使用 Clean Session

                rc_init = MQTTClient_connect(m_pahoClient, &conn_opts);
                if(rc_init != MQTTCLIENT_SUCCESS) {
                    qDebug() << "B MQTT Persistent connect FAILED, rc:" << rc_init;
                    MQTTClient_destroy(&m_pahoClient);
                    m_pahoClient = NULL;
                } else {
                    qDebug() << "B MQTT Persistent connection established to" << uri;
                }
            } else {
                qDebug() << "B MQTT Persistent client create FAILED, rc:" << rc_init;
            }

            // 优化：连接失败时等待 100ms，避免 CPU 占用和 Broker 拒绝
            if (m_pahoClient == NULL) {
                msleep(100);
                continue;
            }
        }
        // ------------------------------------------------------------------

        QDateTime t1 = QDateTime::currentDateTime();

        // 【第二层：数据出队和批量发送循环】
        while(!ProBulkJQs[1].isEmpty()){

            QJsonObject current_u; // 修复内存：将 u 移到最小作用域内

            {
                QMutexLocker locker(&ProBulkJQMutexes[1]);
                if(!ProBulkJQs[1].isEmpty()) current_u = ProBulkJQs[1].dequeue();
            }

            if (current_u.isEmpty()) {
                MpCntEr++;
                continue;
            }
            QByteArray jsonData = QJsonDocument(current_u).toJson(QJsonDocument::Compact);

            // --------------------------------------------------------
            // 【发送逻辑】
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

                    MpCnt++;
                    MpCntt++;

            if (sendMode == SendMode::MQTT) {

                // 【MQTT 高效发送逻辑】
                if (m_pahoClient == NULL) {
                    // 如果客户端在运行中变 NULL，立即退出内循环
                    qWarning() << "B MQTT client became NULL during sending. Resetting.";
                    MpCntEr++;
                    break; // 退出内循环，让外循环进行完整重连
                }

                // 优化：移除内循环的 MQTTClient_isConnected() 检查！
                // 仅信任 publish 的返回码

                QString topic = QString("pduMetaData"); // 建议：使用 "bMetaData" 区分

                int pub_rc = MQTTClient_publish(
                    m_pahoClient, topic.toUtf8().constData(),
                    jsonData.size(), jsonData.constData(),
                    0, false, nullptr
                    );

                if (pub_rc != MQTTCLIENT_SUCCESS) {
                    qWarning() << "B MQTT Publish FAILED, rc:" << pub_rc << ". Performing full client reset.";
                    MpCntEr++;

                    // 核心修复：发送失败，销毁客户端，强制外循环重试
                    MQTTClient_disconnect(m_pahoClient, 500);
                    MQTTClient_destroy(&m_pahoClient);
                    m_pahoClient = NULL;

                    break; // 立即退出数据处理循环
                } else {
                    MpCnt++;
                    MpCntt++;
                }
            } // 结束 MQTT 模式

            // 【限流】 每处理 50 个数据包，短暂休息 1 微秒
            if((cnt++)%50 == 0)
                usleep(1);
        } // 结束 while(!ProBulkJQs[1].isEmpty())

        // 如果 m_pahoClient 在内循环中被置为 NULL，则跳过 usleep(5) 直接进入下一轮重连。
        if (m_pahoClient == NULL && sendMode == SendMode::MQTT) {
            continue;
        }

        // 优化：队列为空时，让出 CPU 给其他线程（包括 A, C, D）
        msleep(5);

        // 计时逻辑 (保持不变)
        if(MpCntt >= (Anum+Bnum+Cnum+Dnum)*0.9) {
            MpCntt = 0;
            QDateTime t2 = QDateTime::currentDateTime();
            int duration = t1.msecsTo(t2);
            //emit TcheckTime(duration);
        }
    }
}

void BMapJsonQueue::BJRun(bool flag)
{
    m_running = flag;
}
