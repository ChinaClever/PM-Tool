#ifndef TRIPHASEJSONQUEUE_H
#define TRIPHASEJSONQUEUE_H

#include <QObject>
#include <QThread>
#include <MQTTClient.h>
class TriPhaseJsonQueue:public QThread
{
    Q_OBJECT
public:
    explicit TriPhaseJsonQueue(QObject* parent = nullptr);
    TriPhaseJsonQueue();
    void run() override;
    ~TriPhaseJsonQueue();
public slots:
    void triRun(bool flag);
signals:
    void TcheckTime(int);
private:

    bool m_running = true;
    MQTTClient m_pahoClient = NULL;
};

#endif // TRIPHASEJSONQUEUE_H
