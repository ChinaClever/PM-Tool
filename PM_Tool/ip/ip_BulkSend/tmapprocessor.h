// tmap_processor.h
#include <QThread>
#include "globals.h"

class TMapProcessor : public QThread {
    Q_OBJECT
public:
    explicit TMapProcessor(QObject* parent = nullptr);
    TMapProcessor();
    void stop();

    void run() override;
    ~TMapProcessor();
    void Incchange(IP_sDataPacket<3>&tMap);
    void PowerCal(IP_sDataPacket<3>&tMap);
    void EleCal(IP_sDataPacket<3>&tMap);
    void updatePhaseValue(double& value, bool& incFlag);
    void updateCurrentPhase(double& value, bool& incFlag, double cap);
    void updatePowerFactor(double& pf, bool& incFlag);
signals:
    void jsonReady(const QByteArray& json);  // 发送序列化后的JSON
public slots:
    void changerun(bool flag);
protected:

private:
    bool m_running = true;
};
