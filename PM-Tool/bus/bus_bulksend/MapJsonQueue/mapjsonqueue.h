#ifndef MAPJSONQUEUE_H
#define MAPJSONQUEUE_H

#include <QObject>
#include <QThread>
#include <MQTTClient.h>

class MapJsonQueue:public QThread
{
    Q_OBJECT
public:
    explicit MapJsonQueue(QObject* parent = nullptr);
    MapJsonQueue();
    ~MapJsonQueue();

    void run() override;
    bool sendMqtt(const QJsonObject &obj, const QString &brokerIp, quint16 brokerPort);
public slots:
    void JRun(bool flag);
signals:
    void checkTime(int);

private:
    bool m_running;
    MQTTClient m_pahoClient = NULL;
};
#endif // MAPJSONQUEUE_H
