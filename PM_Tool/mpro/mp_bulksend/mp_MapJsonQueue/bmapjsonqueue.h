#ifndef BMAPJSONQUEUE_H
#define BMAPJSONQUEUE_H

#include <QObject>
#include <QThread>

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
};

#endif // BMAPJSONQUEUE_H
