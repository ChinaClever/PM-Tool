#ifndef CMAPPROCESSOR_H
#define CMAPPROCESSOR_H
#include "DbWriteThread.h"
#include <QObject>
#include <QThread>
#include <QTimer>
class CMapProcessor:public QThread
{
public:
    explicit CMapProcessor(QObject* parent = nullptr);
    CMapProcessor();
    ~CMapProcessor();
    void run()override;
public slots:
    void CPRun(bool flag);
    void writeDbTimeout();
private:
    bool m_running;
    QTimer* dbWriteTimer;
    QMutex mapMutex; // 用于保护 DMap 访问
    DbWriteThread* dbWriteThread = nullptr;
};

#endif // CMAPPROCESSOR_H
