#ifndef BMAPPROCESSOR_H
#define BMAPPROCESSOR_H
#include "DbWriteThread.h"
#include <QObject>
#include <QThread>
#include <QTimer>
class BMapProcessor:public QThread
{
public:
    explicit BMapProcessor(QObject* parent = nullptr);
    BMapProcessor();
    ~BMapProcessor();
    void run()override;
public slots:
    void BPRun(bool flag);
    void writeDbTimeout();
private:
    bool m_running;
    QTimer* dbWriteTimer;
    QMutex mapMutex; // 用于保护 DMap 访问
    DbWriteThread* dbWriteThread = nullptr;
};

#endif // BMAPPROCESSOR_H
