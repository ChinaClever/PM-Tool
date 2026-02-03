#ifndef AMAPPROCESSOR_H
#define AMAPPROCESSOR_H
#include "DbWriteThread.h"
#include <QThread>
#include <QObject>
#include <QTimer>
class AMapProcessor: public QThread
{
    Q_OBJECT
public:
    explicit AMapProcessor(QObject* parent = nullptr);
    AMapProcessor();
    ~AMapProcessor();
    void run() override;

public slots:
    void APRun(bool flag);
    void writeDbTimeout();
signals:
    void checkSend(int,int,int);

private:
    bool m_running;
    QTimer* dbWriteTimer;
    QMutex mapMutex; // 用于保护 DMap 访问
    DbWriteThread* dbWriteThread = nullptr;
};

#endif // AMAPPROCESSOR_H
