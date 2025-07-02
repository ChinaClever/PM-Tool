#pragma once
#include <QThread>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include "DbWriteTask.h"

class DbWriteThread : public QThread
{
    Q_OBJECT
public:
    explicit DbWriteThread(QObject *parent = nullptr);
    ~DbWriteThread();

    void enqueueTask(const DbWriteTask& task);

protected:
    void run() override;

private:
    QQueue<DbWriteTask> m_taskQueue;
    QMutex m_mutex;
    QWaitCondition m_cond;
    bool m_stopped = false;
};
