#ifndef TRIPHASEJSONQUEUE_H
#define TRIPHASEJSONQUEUE_H

#include <QObject>
#include <QThread>
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
private:
    bool m_running = true;
};

#endif // TRIPHASEJSONQUEUE_H
