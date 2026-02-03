#ifndef SRIPHASEJSONQUEUE_H
#define SRIPHASEJSONQUEUE_H
#include <QObject>
#include <QThread>
#include <MQTTClient.h>

class SriPhaseJsonQueue:public QThread
{
    Q_OBJECT
public:
    explicit SriPhaseJsonQueue(QObject* parent = nullptr);
    SriPhaseJsonQueue();
    void run() override;
    ~SriPhaseJsonQueue();
public slots:
    void sriRun(bool flag);
signals:
    void ScheckTime(int);
private:
    bool m_running;
    MQTTClient m_pahoClient = NULL;

};

#endif // SRIPHASEJSONQUEUE_H
