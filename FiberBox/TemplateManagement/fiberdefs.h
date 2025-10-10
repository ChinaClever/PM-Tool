#ifndef FIBERDEFS_H
#define FIBERDEFS_H

#include <QString>
// Interface
enum class InterfaceType {
    MTP_LC,  //  MTP-LC
    MTP_SN,  //  MTP-SN
    MTP_MDC,  //  MTP-MDC
    Unknown
};

// FiberCount
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
    UNIVERSAL,  // 通用 UNIVERSAL
    AC,
    Unknown
};

enum class FiberMode {
    SM,
    MM,
    Unknown
};

struct sFiberInfo {
    InterfaceType iface;
    FiberCountType count; //FiberCount
    FiberSpec spec;
    Polarity polarity;
    FiberMode mode;   //SM/MM
};


inline QString ifaceToString(InterfaceType i)
{
    switch(i){
    case InterfaceType::MTP_LC : return "MTP-LC";
    case InterfaceType::MTP_MDC: return "MTP-MDC";
    case InterfaceType::MTP_SN : return "MTP-SN";
    default: return "Unknown";
    }
}

inline QString countToString(FiberCountType c)
{
    switch(c){
    case FiberCountType::F8 : return "8F";
    case FiberCountType::F12: return "12F";
    case FiberCountType::F16: return "16F";
    default: return "Unknown";
    }
}

inline QString specToString(FiberSpec s)
{
    switch(s){
    case FiberSpec::OS2     : return "OS2";
    case FiberSpec::OM5     : return "OM5";
    case FiberSpec::OM4_Aqua: return "OM4 Aqua";
    case FiberSpec::OM4_HV  : return "OM4 HV";
    default: return "Unknown";
    }
}

inline QString polarityToString(Polarity p)
{
    switch(p){
    case Polarity::UNIVERSAL: return "UNIVERSAL";
    case Polarity::AC :       return "AC";
    default: return "Unknown";
    }
}

inline QString modeToString(FiberMode m)
{
    switch(m){
    case FiberMode::MM : return "MM";
    case FiberMode::SM : return "SM";
    default: return "Unknown";
    }
}

#endif // FIBERDEFS_H
