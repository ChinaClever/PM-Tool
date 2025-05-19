#ifndef SMAPPROCESSOR_H
#define SMAPPROCESSOR_H

#include <QThread>
#include "globals.h"

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

    void EleCal(IP_sDataPacket<1>&sMap);
    void PowerCal(IP_sDataPacket<1>&sMap);
public slots:
    void Schangerun(bool flag);
private:
    bool m_running = true;
};

#endif // SMAPPROCESSOR_H
