// tmap_processor.h
#include <QThread>
#include "globals.h"
#include "DbWriteThread.h"
#include <QTimer>
class TMapProcessor : public QThread {
    Q_OBJECT
public:
    explicit TMapProcessor(QObject* parent = nullptr);
    TMapProcessor();
    ~TMapProcessor();

    void run() override;
    void Incchange(IP_sDataPacket<3>&tMap); //增量更新数值
    void PowerCal(IP_sDataPacket<3>&tMap);
    void EleCal(IP_sDataPacket<3>&tMap,QString &key);
    void updatePhaseValue(double& value, bool& incFlag);
    void updateCurrentPhase(double& value, bool& incFlag, double cap);
    void updatePowerFactor(double& pf, bool& incFlag);

public slots:
    void Tchangerun(bool flag);
    void onSaveTimerTimeout();
signals:
    void TcheckSend(int,int,int);

private:
    bool m_running = true;
    QTimer* mSaveTimer = nullptr;
    QMutex mMapMutex;  // 如果你有多线程操作 sMap，最好用锁保护

    DbWriteThread* dbWriteThread = nullptr;
};
