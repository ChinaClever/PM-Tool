#include "DbWriteThread.h"
#include "DatabaseManager.h"
#include <QDebug>

DbWriteThread::DbWriteThread(QObject *parent) : QThread(parent) {}

DbWriteThread::~DbWriteThread() {
    m_mutex.lock();
    m_stopped = true;
    m_cond.wakeAll();
    m_mutex.unlock();
    wait();
}

void DbWriteThread::enqueueTask(const DbWriteTask &task) {
    QMutexLocker locker(&m_mutex);

    // 如果队列过长，清空旧任务，只保留最新任务
    if (m_taskQueue.size() >= 6000) {
        qDebug() << "⚠️ DbWriteThread 队列超过 6000，清空旧任务以释放内存";
        m_taskQueue.clear();
    }

    m_taskQueue.enqueue(task);

    m_cond.wakeOne();
}


void DbWriteThread::run() {
    while (true) {
        m_mutex.lock();
        while (m_taskQueue.isEmpty() && !m_stopped) {
            m_cond.wait(&m_mutex);
        }
        if (m_stopped) {
            m_mutex.unlock();
            break;
        }
        if (m_taskQueue.isEmpty()) {
            m_mutex.unlock();
            continue;
        }
        DbWriteTask task = m_taskQueue.dequeue();
        m_mutex.unlock();

        bool ok = false;
        switch (task.table) {
        case DbWriteTask::ThreePhase:
            if (task.values.size() >= 3)
                ok = DatabaseManager::instance().insertOrUpdateThreePhaseEnergy(
                    task.key,
                    task.values[0].toDouble(),
                    task.values[1].toDouble(),
                    task.values[2].toDouble());
            break;
        case DbWriteTask::SinglePhase:
            if (task.values.size() >= 1)
                ok = DatabaseManager::instance().insertOrUpdateSignalPhaseEnergy(
                    task.key,
                    task.values[0].toDouble());
            break;
        case DbWriteTask::BoxPhase:
            if (task.values.size() >= 18) { // 9+9
                double eleActive[9], eleReactive[9];
                for (int i = 0; i < 9; ++i) eleActive[i] = task.values[i].toDouble();
                for (int i = 0; i < 9; ++i) eleReactive[i] = task.values[i + 9].toDouble();

                //for(int i = 0;i<9;i++)qDebug()<<eleActive[i];
               // for(int i = 0;i<9;i++)qDebug()<<eleReactive[i];

                ok = DatabaseManager::instance().insertOrUpdateBoxPhaseEnergy(task.key, eleActive, eleReactive);
            }
            break;
        case DbWriteTask::OutputBit:
            if (task.values.size() >= 48) {
                double energy[48];
                for (int i = 0; i < 48; ++i) energy[i] = task.values[i].toDouble();
                ok = DatabaseManager::instance().insertOrUpdateOutputBitEnergy(task.key, energy);
            }
            break;
        }

        if (!ok) {
            qDebug() << "数据库写入失败, 表:" << task.table << ", key:" << task.key << ", values.size:" << task.values.size();
        }
    }
}

