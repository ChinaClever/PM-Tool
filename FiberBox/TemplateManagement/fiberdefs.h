#ifndef FIBERDEFS_H
#define FIBERDEFS_H

#include <QString>

// ---------------------------
// 枚举定义
// ---------------------------
enum class InterfaceType {
    MTP_LC,   // MTP-LC
    MTP_SN,   // MTP-SN
    MTP_MDC,  // MTP-MDC
    Unknown
};

enum class FiberCountType {
    F8  = 8,
    F12 = 12,
    F16 = 16,
    Unknown
};

enum class FiberSpec {
    OS2,
    OM4_Aqua,
    OM4_HV,
    OM5,
    Unknown
};

enum class Polarity {
    Universal,
    AC,
    Unknown
};

enum class FiberMode {
    SM,
    MM,
    Unknown
};

// ---------------------------
// 光纤结构信息
// ---------------------------
struct sFiberInfo {
    InterfaceType iface = InterfaceType::Unknown;
    FiberCountType count = FiberCountType::Unknown;
    FiberSpec spec = FiberSpec::Unknown;
    Polarity polarity = Polarity::Unknown;
    FiberMode mode = FiberMode::Unknown;
};
// ---------------------------
// 模板信息
// ---------------------------
struct sTemInfo {
    QString PN;             // 模板编码
    QString FanoutPn;       // 扇出线物料编码
    int FanCount = 1;       // 扇出线数量
    double limit = 0.0;     // 最大插入损耗
    QString description;    // 模板描述
    sFiberInfo info;        // 光纤信息

    std::pair<int,int>lambda; //

    QString qrTemplate;     // 二维码模板    --TemplateA(B、C)
    QString labelTemplate;  // 标签模板ID    --Template1(2、3)
};

// ---------------------------
// 字符串转换函数（双向）
// ---------------------------

// ---- InterfaceType ----
inline QString ifaceToString(InterfaceType i) {
    switch(i){
    case InterfaceType::MTP_LC:  return "MTP-LC";
    case InterfaceType::MTP_SN:  return "MTP-SN";
    case InterfaceType::MTP_MDC: return "MTP-MDC";
    default: return "Unknown";
    }
}

inline InterfaceType stringToIface(const QString& s) {
    if (s == "MTP-LC")  return InterfaceType::MTP_LC;
    if (s == "MTP-SN")  return InterfaceType::MTP_SN;
    if (s == "MTP-MDC") return InterfaceType::MTP_MDC;
    return InterfaceType::Unknown;
}

// ---- FiberCountType ----
inline QString countToString(FiberCountType c) {
    switch(c){
    case FiberCountType::F8:  return "8F";
    case FiberCountType::F12: return "12F";
    case FiberCountType::F16: return "16F";
    default: return "Unknown";
    }
}

inline FiberCountType stringToCount(const QString& s) {
    if (s == "8F")  return FiberCountType::F8;
    if (s == "12F") return FiberCountType::F12;
    if (s == "16F") return FiberCountType::F16;
    return FiberCountType::Unknown;
}


// ---- FiberSpec ----
inline QString specToString(FiberSpec s) {
    switch(s){
    case FiberSpec::OS2:       return "OS2";
    case FiberSpec::OM4_Aqua:  return "OM4 Aqua";
    case FiberSpec::OM4_HV:    return "OM4 HV";
    case FiberSpec::OM5:       return "OM5";
    default: return "Unknown";
    }
}

inline FiberSpec stringToSpec(const QString& s) {
    if (s == "OS2")       return FiberSpec::OS2;
    if (s == "OM4 Aqua")  return FiberSpec::OM4_Aqua;
    if (s == "OM4 HV")    return FiberSpec::OM4_HV;
    if (s == "OM5")       return FiberSpec::OM5;
    return FiberSpec::Unknown;
}

// ---- Polarity ----
inline QString polarityToString(Polarity p) {
    switch(p){
    case Polarity::Universal: return "Universal";
    case Polarity::AC:        return "A/C";
    default: return "Unknown";
    }
}

inline Polarity stringToPolarity(const QString& s) {
    if (s == "Universal") return Polarity::Universal;
    if (s == "A/C")        return Polarity::AC;
    return Polarity::Unknown;
}

// ---- FiberMode ----
inline QString modeToString(FiberMode m) {
    switch(m){
    case FiberMode::SM: return "SM";
    case FiberMode::MM: return "MM";
    default: return "Unknown";
    }
}

inline FiberMode stringToMode(const QString& s) {
    if (s == "SM") return FiberMode::SM;
    if (s == "MM") return FiberMode::MM;
    return FiberMode::Unknown;
}

inline QString modeToWaveLength(FiberMode m)
{
    switch (m) {
    case FiberMode::SM: return "1310nm/1550nm";
    case FiberMode::MM: return "850nm/1300nm";
    default: return "Unknown";
    }
}


#endif // FIBERDEFS_H
