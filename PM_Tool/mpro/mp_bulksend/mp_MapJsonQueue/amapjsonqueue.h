#ifndef AMAPJSONQUEUE_H
#define AMAPJSONQUEUE_H

#include <QObject>
#include <QThread>
#include <MQTTClient.h>

class AMapJsonQueue:public QThread
{
    Q_OBJECT
public:
    explicit AMapJsonQueue(QObject* parent = nullptr);
    AMapJsonQueue();
    ~AMapJsonQueue();

    void run() override;
public slots:
    void AJRun(bool flag);

private:
    bool m_running;
    MQTTClient m_pahoClient = NULL;
};

#endif // AMAPJSONQUEUE_H
