#ifndef DMAPJSONQUEUE_H
#define DMAPJSONQUEUE_H

#include <QObject>
#include <QThread>

class DMapJsonQueue:public QThread
{
public:
    explicit DMapJsonQueue(QObject *parent = nullptr);
    DMapJsonQueue();
    ~DMapJsonQueue();
    void run()override;
public slots:
    void DJRun(bool flag);
private:
    bool m_running;
};

#endif // DMAPJSONQUEUE_H
