// tmap_processor.h
#include <QThread>
#include "globals.h"

class TMapProcessor : public QThread {
    Q_OBJECT
public:
    explicit TMapProcessor(QObject* parent = nullptr);
    TMapProcessor();
    ~TMapProcessor();

    void run() override;
    void Incchange(IP_sDataPacket<3>&tMap); //增量更新数值
    void PowerCal(IP_sDataPacket<3>&tMap);
    void EleCal(IP_sDataPacket<3>&tMap);
    void updatePhaseValue(double& value, bool& incFlag);
    void updateCurrentPhase(double& value, bool& incFlag, double cap);
    void updatePowerFactor(double& pf, bool& incFlag);

public slots:
    void Tchangerun(bool flag);
protected:

private:
    bool m_running = true;
};
