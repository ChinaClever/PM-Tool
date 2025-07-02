#ifndef SRIPHASEJSONQUEUE_H
#define SRIPHASEJSONQUEUE_H
#include <QObject>
#include <QThread>

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
private:
    bool m_running;



};

#endif // SRIPHASEJSONQUEUE_H
