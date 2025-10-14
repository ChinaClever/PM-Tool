// TemplateManagement/fiberdefs.h (完整版本)
#ifndef FIBERDEFS_H
#define FIBERDEFS_H

#include <QString>
#include <QMap>
#include <QVariant>
#include <QRegularExpression>
#include <QList>

// Interface - 保持原有枚举
enum class InterfaceType {
    MTP_LC,  //  MTP-LC
    MTP_SN,  //  MTP-SN
    MTP_MDC,  //  MTP-MDC
    Unknown
};

// FiberCount - 保持原有枚举
enum class FiberCountType {
    F8  = 8,
    F12 = 12,
    F16 = 16,
    Unknown
};

// 保留原有的其他枚举
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

// 类型解析器类
class TypeParser {
public:
    // 解析类型字符串，提取接口类型和光纤数量
    static bool parseType(const QString& typeString, InterfaceType& interface, FiberCountType& fiberCount) {
        QRegularExpression regex(R"((\w+(?:-\w+)?)\s+(\d+)F?)");
        QRegularExpressionMatch match = regex.match(typeString);

        if (match.hasMatch()) {
            QString interfaceStr = match.captured(1);
            int count = match.captured(2).toInt();

            // 解析接口类型
            interface = parseInterfaceType(interfaceStr);

            // 解析光纤数量
            fiberCount = parseFiberCount(count);

            return (interface != InterfaceType::Unknown && fiberCount != FiberCountType::Unknown);
        }

        interface = InterfaceType::Unknown;
        fiberCount = FiberCountType::Unknown;
        return false;
    }

    // 组合类型字符串
    static QString composeType(InterfaceType interface, FiberCountType fiberCount) {
        QString interfaceStr = interfaceToString(interface);
        int count = static_cast<int>(fiberCount);
        return QString("%1 %2F").arg(interfaceStr).arg(count);
    }

    // 验证类型字符串是否有效
    static bool isValidType(const QString& typeString) {
        InterfaceType interface;
        FiberCountType fiberCount;
        return parseType(typeString, interface, fiberCount);
    }

private:
    static InterfaceType parseInterfaceType(const QString& str) {
        if (str == "MTP-LC") return InterfaceType::MTP_LC;
        if (str == "MTP-SN") return InterfaceType::MTP_SN;
        if (str == "MTP-MDC") return InterfaceType::MTP_MDC;
        return InterfaceType::Unknown;
    }

    static FiberCountType parseFiberCount(int count) {
        switch (count) {
        case 8: return FiberCountType::F8;
        case 12: return FiberCountType::F12;
        case 16: return FiberCountType::F16;
        default: return FiberCountType::Unknown;
        }
    }

    static QString interfaceToString(InterfaceType type) {
        switch (type) {
        case InterfaceType::MTP_LC: return "MTP-LC";
        case InterfaceType::MTP_SN: return "MTP-SN";
        case InterfaceType::MTP_MDC: return "MTP-MDC";
        default: return "Unknown";
        }
    }
};

struct sFiberInfo {
    InterfaceType iface;
    FiberCountType count;
    FiberSpec spec;
    Polarity polarity;
    FiberMode mode;
    std::pair<int,int>landa;

    // 接口类型
    QString typeString;  // 如 "MTP-LC 16F"

    sFiberInfo() : iface(InterfaceType::Unknown), count(FiberCountType::Unknown),
        spec(FiberSpec::Unknown), polarity(Polarity::Unknown), mode(FiberMode::Unknown) {}

    // 新增方法
    void setFromTypeString(const QString& typeStr) {
        typeString = typeStr;
        TypeParser::parseType(typeStr, iface, count);
    }

    void setFromEnums(InterfaceType interface, FiberCountType fiberCount) {
        iface = interface;
        count = fiberCount;
        typeString = TypeParser::composeType(interface, fiberCount);
    }

    QString getTypeString() const {
        if (typeString.isEmpty()) {
            return TypeParser::composeType(iface, count);
        }
        return typeString;
    }

    bool isTypeStringValid() const {
        if (typeString.isEmpty()) return true;
        return TypeParser::isValidType(typeString);
    }
};

// 保持原有的转换函数
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

// 扩展的模板信息结构体
struct sTemInfo{
    QString PN;
    QString FanoutPn;
    sFiberInfo info;
    QString description;

    QString qrTemplate;
    int labelTemplate;

    int FanCount;               // 用量
    double limit;                 // 限值

    // 构造函数
    sTemInfo() : FanCount(0), limit(0.0) {}

    // 新增：获取类型字符串
    QString getTypeString() const {
        return info.getTypeString();
    }

    // 新增：设置类型字符串
    void setTypeString(const QString& typeStr) {
        info.setFromTypeString(typeStr);
    }

    // 新增：验证方法
    bool isValid() const {
        return !PN.isEmpty() && !description.isEmpty() && info.isTypeStringValid();
    }

    // 新增：转换为显示字符串
    QString toString() const {
        return QString("PN: %1, 类型: %2, 描述: %3").arg(PN, getTypeString(), description);
    }
};


#endif // FIBERDEFS_H
