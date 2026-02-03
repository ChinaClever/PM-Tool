#ifndef DMAPPROCESSOR_H
#define DMAPPROCESSOR_H
#include "DbWriteThread.h"
#include <QObject>
#include <QThread>
#include <QTimer>
#include <QMutex>

class DMapProcessor:public QThread
{
public:
    explicit DMapProcessor(QObject* parent = nullptr);
    DMapProcessor();
    ~DMapProcessor();
    void run()override;
public slots:
    void DPRun(bool flag);
    void writeDbTimeout();
private:
    bool m_running;
    QTimer* dbWriteTimer;
    QMutex mapMutex; // 用于保护 DMap 访问
    DbWriteThread* dbWriteThread = nullptr;

};

#endif // DMAPPROCESSOR_H
