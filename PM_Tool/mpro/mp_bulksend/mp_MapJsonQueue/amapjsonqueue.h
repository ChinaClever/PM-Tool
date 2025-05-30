#ifndef AMAPJSONQUEUE_H
#define AMAPJSONQUEUE_H

#include <QObject>
#include <QThread>

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
};

#endif // AMAPJSONQUEUE_H
