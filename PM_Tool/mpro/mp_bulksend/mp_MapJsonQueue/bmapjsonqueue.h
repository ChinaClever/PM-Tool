#ifndef BMAPJSONQUEUE_H
#define BMAPJSONQUEUE_H

#include <QObject>
#include <QThread>
#include <MQTTClient.h>
class BMapJsonQueue:public QThread
{
public:
    explicit BMapJsonQueue(QObject *parent = nullptr);
    BMapJsonQueue();
    ~BMapJsonQueue();
    void run()override;
public slots:
    void BJRun(bool flag);
private:
    bool m_running;
    MQTTClient m_pahoClient = NULL;
};

#endif // BMAPJSONQUEUE_H
