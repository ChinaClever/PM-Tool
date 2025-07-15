#ifndef SMAPPROCESSOR_H
#define SMAPPROCESSOR_H

#include <QThread>
#include "globals.h"
#include <QTimer>
#include "DbWriteThread.h"

class SMapProcessor: public QThread {
    Q_OBJECT
public:
    explicit SMapProcessor(QObject* parent = nullptr);
    SMapProcessor();
    ~SMapProcessor();

    void run() override;
    void Incchange(IP_sDataPacket<1>&sMap);
    void updatePhaseValue(double& value, bool& incFlag);
    void updateCurrentPhase(double& value, bool& incFlag, double cap);
    void updatePowerFactor(double& pf, bool& incFlag);

    void EleCal(IP_sDataPacket<1>&sMap,const QString& key);
    void PowerCal(IP_sDataPacket<1>&sMapm);
public slots:
    void Schangerun(bool flag);
    void onSaveTimerTimeout();
signals:
    void ScheckSend(int,int,int);
private:
    bool m_running = true;
    QTimer* mSaveTimer = nullptr;
    QMutex mMapMutex;  // 如果你有多线程操作 sMap，最好用锁保护

    DbWriteThread* dbWriteThread = nullptr;
};

#endif // SMAPPROCESSOR_H
