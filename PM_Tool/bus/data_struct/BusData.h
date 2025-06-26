#pragma once
#include <QVector>
#include <QString>
#include <QDateTime>

// 线路数据项（母线）
struct LineItem {
    QVector<double> volValue;        // 电压值数组
    QVector<int> volMin;          // 电压下限
    QVector<int> volMax;          // 电压上限
    QVector<int> volStatus;       // 电压告警状态
    QVector<int> volThd;          // 电压谐波含量

    QVector<double> curValue;        // 电流值
    QVector<int> curMin;          // 电流下限
    QVector<int> curMax;          // 电流上限
    QVector<int> curStatus;       // 电流告警状态
    QVector<int> curThd;          // 电流谐波含量

    QVector<double> powValue;        // 有功功率
    QVector<int> powMin;          // 有功下限
    QVector<int> powMax;          // 有功上限
    QVector<int> powStatus;       // 功率告警状态

    QVector<double> powApparent;     // 视在功率
    QVector<double> powReactive;     // 无功功率
    QVector<double> eleActive;       // 有功电能
    QVector<double> eleReactive;     // 无功电能
    QVector<double> powerFactor;     // 功率因素
    QVector<double> loadRate;        // 负载率

    QVector<double> volLineValue;    // 线电压值
    QVector<int> volLineMin;      // 线电压下限
    QVector<int> volLineMax;      // 线电压上限
    QVector<int> volLineStatus;   // 线电压状态

    void init(int size ) {
        volValue.resize(size); volMin.resize(size); volMax.resize(size);
        volStatus.resize(size); volThd.resize(size);
        curThd = QVector<int>(size);

        curValue.resize(size); curMin.resize(size); curMax.resize(size);
        curStatus.resize(size);

        powValue.resize(size); powMin.resize(size); powMax.resize(size);
        powStatus.resize(size);

        powApparent.resize(size); powReactive.resize(size); eleActive.resize(size);
        eleReactive.resize(size); powerFactor.resize(size); loadRate.resize(size);

        volLineValue.resize(size); volLineMin.resize(size); volLineMax.resize(size);
        volLineStatus.resize(size);
    }
};

// 母线配置参数
struct BusConfig {
    int curSpecs;              // 电流规格
    int workMode;              // 工作模式
    int beep;                  // 蜂鸣器状态
    int acDc;                  // 交直流类型
    int boxNum;                // 插接箱数量
    int iof;                   // iof触点
    int isd;                   // isd报警触点
    int shuntTrip;             // 分励脱扣
    int breakerStatus;         // 断路器状态
    int lspStatus;             // 防雷状态
    int busVersion;            // 软件版本
    int itemType;              // 项目类型
    int baudRate;              // 波特率
    int alarmCount;            // 告警滤波次数
    int vol;
};

// 母线总数据
struct BusTotalData {
    double powValue;              // 总有功功率
    int powMin;                // 总有功下限
    int powMax;                // 总有功上限
    int powStatus;             // 总功率状态

    double powApparent;           // 总视在功率
    double powReactive;           // 总无功功率
    double powerFactor;           // 功率因素
    double eleActive;             // 总电能
    double eleApparent;           // 总视在电能
    double eleReactive;           // 总无功电能

    double curResidualValue;      // 剩余电流值
    int curResidualAlarm;      // 剩余电流报警值
    int curResidualStatus;     // 剩余电流状态

    double curZeroValue;          // 零线电流
    int curZeroAlarm;          // 零线报警值
    int curZeroStatus;         // 零线状态

    double volUnbalance;          // 电压不平衡
    double curUnbalance;          // 电流不平衡
    double hzValue;               // 频率值
    double hzMin;                 // 频率下限
    double hzMax;                 // 频率上限
    int hzStatus;              // 频率状态
};

// 母线数据结构
struct BusData {
    BusConfig busCfg;
    LineItem lineItemList;
    BusTotalData busTotalData;

    void init(int lineSize) {
        lineItemList.init(lineSize);
    }
};

// 插接箱配置
struct BoxConfig {
    int alarmCount;            // 告警次数
    int iof;                   // iof触点
    QString boxVersion;            // 软件版本
    int baudRate;              // 波特率
    int beep;                  // 蜂鸣器
    int itemType;              // 项目类型
    int workMode;              // 工作模式
    int loopNum;               // 回路数量
    int boxType;               // 盒子类型
    QVector<int> breakerStatus;  // 断路器状态数组

    void init(int breakerSize) {
        breakerStatus.resize(breakerSize);
    }
};

// 回路数据项
struct LoopItem {
    QVector<int> volValue;       // 电压值
    QVector<int> volMin;         // 电压下限
    QVector<int> volMax;         // 电压上限
    QVector<int> volStatus;      // 电压状态

    QVector<double> curValue;       // 电流值
    QVector<int> curMin;         // 电流下限
    QVector<int> curMax;         // 电流上限
    QVector<int> curStatus;      // 电流状态
    QVector<bool>curInc;
    QVector<double> powValue;       // 有功功率
    QVector<int> powMin;         // 功率下限
    QVector<int> powMax;         // 功率上限
    QVector<int> powStatus;      // 功率状态

    QVector<double> powReactive;    // 无功功率
    QVector<double> powApparent;    // 视在功率
    QVector<double> powerFactor;    // 功率因素
    QVector<bool> pfInc;
    QVector<double> eleActive;      // 电能
    QVector<double> eleReactive;    // 无功电能

    void init(int size) {
        volValue.resize(size); volMin.resize(size); volMax.resize(size);
        volStatus.resize(size);
        curInc.resize(size);
        pfInc.resize(size);
        curValue.resize(size); curMin.resize(size); curMax.resize(size);
        curStatus.resize(size);

        powValue.resize(size); powMin.resize(size); powMax.resize(size);
        powStatus.resize(size);

        powReactive.resize(size); powApparent.resize(size);
        powerFactor.resize(size); eleActive.resize(size);
        eleReactive.resize(size);
    }
};

// 线路数据（插接箱）
struct BoxLineItem {
    QVector<int> volValue;       // 电压值
    QVector<double> curValue;       // 电流值
    QVector<int> curThd;         // 电流谐波

    QVector<double> powActive;      // 有功功率
    QVector<double> powApparent;    // 视在功率
    QVector<double> powReactive;    // 无功功率
    QVector<double> loadRate;       // 负载率

    QVector<double> eleActive;      // 电能
    QVector<double> eleReactive;    // 无功电能
    QVector<double> powerFactor;    // 功率因素

    void init(int size) {
        volValue.resize(size); curValue.resize(size); curThd.resize(size);
        powActive.resize(size); powApparent.resize(size); powReactive.resize(size);
        loadRate.resize(size); eleActive.resize(size); eleReactive.resize(size);
        powerFactor.resize(size);
    }
};

// 输出项数据
struct OutletItem {
    QVector<double> powActive;      // 有功功率
    QVector<double> powApparent;    // 视在功率
    QVector<double> powReactive;    // 无功功率

    QVector<double> eleActive;      // 电能
    QVector<double> eleApparent;    // 视在电能
    QVector<double> eleReactive;    // 无功电能
    QVector<double> powerFactor;    // 功率因素

    void init(int size) {
        powActive.resize(size); powApparent.resize(size); powReactive.resize(size);
        eleActive.resize(size); eleApparent.resize(size); eleReactive.resize(size);
        powerFactor.resize(size);
    }
};

// 插接箱总数据
struct BoxTotalData {
    double powActive;             // 总有功
    double powApparent;           // 总视在
    double powReactive;           // 总无功
    double powerFactor;           // 功率因素
    double eleActive;             // 总电能
    double eleApparent;           // 总视在电能
    double eleReactive;           // 总无功电能
};

// 插接箱数据结构
struct BoxData {
    BoxConfig boxCfg;
    LoopItem loopItemList;
    BoxLineItem lineItemList;
    OutletItem outletItemList;
    BoxTotalData boxTotalData;


};

// 环境数据
struct EnvItem {
    QVector<double> temValue;       // 温度值数组
    QVector<double> boxtem;
    QVector<int> temStatus;      // 温度状态
    QVector<int> boxStatus;      // 温度状态
    void init(int size ) {
        temValue.resize(size); boxStatus.resize(size); boxtem.resize(size);
        temStatus.resize(size);
    }
};

struct Info{
    int addr;                  // 地址
    int status;                // 状态
    QString devIp;             // 设备IP
    QString key;
    QString datetime;        // 当前时间
    int barId;               // 母线ID
    QString devAlarm;          // 告警信息
    QString busName;           // 母线名称
    QString boxName;           // 插接箱名称
    QString busKey;            // 母线键
    QString boxKey;            // 插接箱键
};

// 主结构体
struct  Busbar{

    QString m_ser_ip;
    QString m_port;
    bool flag;


    Info info;
    BusData busData;
    BoxData boxData;
    EnvItem envItemList;

    void init(int lineSize = 3, int envSize = 4) {
        busData.init(lineSize);

        envItemList.init(envSize);
    }
};
