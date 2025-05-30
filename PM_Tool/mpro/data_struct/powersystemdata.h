#ifndef POWERSYSTEMDATA_H
#define POWERSYSTEMDATA_H

#include <QVector>
#include <QString>
#include <QDateTime>

// 环境数据
struct EnvironmentData {
    QVector<double> temperatures;  // 4个温度值
    QVector<double> humidities;   // 4个湿度值
    QVector<bool> sensorStatuses; // 4个传感器状态
    QVector<double> dewPoints;    // 4个露点温度
    QVector<int> hunAlarm = {0};
    QVector<int> temAlarm = {0};
    QVector<bool> incr ={0};
    QVector<bool> humincr ={0};
    void initialize(int count) {
        temperatures.resize(count);
        humidities.resize(count);
        sensorStatuses.resize(count);
        dewPoints.resize(count);
        hunAlarm.resize(count);
        temAlarm.resize(count);
        incr.resize(count);
        humincr.resize(count);
    }
};

// 输出位数据
struct OutputBit {

    double curAlarmMax;  //电流上限
    int curAlarmStatus;    //电流报警状态
    int powAlarmStatus;
    bool switchStatus = 1;      // 开关状态
    double current;         // 电流(A)
    double vol;             // 电压
    double activePower;     // 有功功率
    double reactivePower;   // 无功功率
    double apparentPower;   // 视在功率
    double powerFactor;     // 功率因数
    double energy;          // 电能
    double currentIncrement;// 电流增量

    bool curinc = 1;//电流增量
    bool pfinc = 1; //功率因素增量
};

// 回路数据
struct Circuit {
    bool breakerStatus;     // 断路器状态
    double voltage;         // 电压
    double current;         // 电流
    double activePower;     // 有功功率
    double reactivePower;   // 无功功率
    double apparentPower;   // 视在功率
    double powerFactor;     // 功率因数
    double energy;          // 电能
    double curAlarmMax;     //电流上限
    int curAlarmStatus;     //电流报警状态
    int powAlarmStatus;     //功率报警状态
    int volAlarmStatus;     //电压报警状态

};

// 相位数据
struct PhaseData {
    QVector<double> voltages;    // 电压数组
    QVector<double> currents;    // 电流数组
    QVector<double> activePowers; // 有功功率数组
    QVector<double> reactivePowers; // 无功功率数组
    QVector<double> apparentPowers; // 视在功率数组
    QVector<double> powerFactors; // 功率因数数组
    QVector<double> energies;     // 电能数组
    QVector<double> phase_voltage;
    QVector<int> volStatus;
    QVector<int> curStatus;
    QVector<int> curMax;


    int phaseNum;

    // 初始化函数
    void initialize(bool isSinglePhase) {
        int count = isSinglePhase ? 1 : 3;
        phaseNum =count;
        voltages.resize(count);
        currents.resize(count);
        activePowers.resize(count);
        reactivePowers.resize(count);
        apparentPowers.resize(count);
        powerFactors.resize(count);
        energies.resize(count);
        phase_voltage.resize(count);
        volStatus.resize(count);
        curStatus.resize(count);
        curMax.resize(count);
    }
};

// 阈值设置
struct CapData {
    double capVol;          // 电压阈值
    double capCirCur;       // 回路电流阈值
    double capPhaseCur;     // 相位电流阈值
    double capbitCur;       // 输出位电流阈值
};

// 系统设置
struct SystemSettings {
    QString series;          // 系列(A/B/C/D)
    QString phaseMode;       // 相位模式(单相/三相)
    int outputBitCount;      // 输出位数量
    int circuitCount;        // 回路数量

};

// 告警信息
struct AlarmInfo {
    QString description;     // 告警描述
    double currentValue;     // 当前值
    double alarmMin;        // 告警最小值
    double warnMin;         // 预警最小值
    double warnMax;         // 预警最大值
    double alarmMax;        // 告警最大值
};

// 完整的电力系统数据结构
struct PowerSystemData {
    // 设备基本信息

    int addr = 0;
    int status = 0;
    double devHz = 0;
    int version = 2;

    QString devIp;
    QString dev_key;

    QString datetime;
    QString uuid;


    void initializeInfo(const QString& devip, const int& Addr, const QString& key){
        devIp = devip; addr = Addr; dev_key = key;
        devHz = 55;
        version = 2;
        datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    }

    // 设备类型信息
    struct {
        QString pduType;
        QString uuid;
    } uutInfo;

    // 告警信息
    QVector<AlarmInfo> pduAlarms;

    // 监测数据
    struct {
        EnvironmentData envData;

        PhaseData phases;


        struct {
            QVector<Circuit> Circuits;
        } loopData;

        struct {
            QVector<OutputBit> outputBits;
        } outputData;

        struct {
            double curUnbalance;
            double curValue;
            double eleActive;
            double powActive;
            double powApparent;
            double powReactive;
            double powerFactor;
            double volUnbalance;
        } totalData;
    } pduData;

    // 系统设置
    SystemSettings settings;

    // 阈值设置
    CapData thresholds;
};

#endif // POWERSYSTEMDATA_H
