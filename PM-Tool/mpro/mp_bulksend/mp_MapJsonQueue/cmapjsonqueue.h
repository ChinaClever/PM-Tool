#ifndef CMAPJSONQUEUE_H
#define CMAPJSONQUEUE_H

#include <QObject>
#include <QThread>
#include <MQTTClient.h>
class CMapJsonQueue:public QThread
{
public:
    explicit CMapJsonQueue(QObject *parent = nullptr);
    CMapJsonQueue();
    ~CMapJsonQueue();
    void run()override;
public slots:
    void CJRun(bool flag);
private:
    bool m_running;
    MQTTClient m_pahoClient = NULL;
};

#endif // CMAPJSONQUEUE_H
