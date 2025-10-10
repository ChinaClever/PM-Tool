#include "fibeltem.h"
#include <QDebug>
FiberTem::FiberTem(){}
FiberTem::~FiberTem(){}

FiberTem& FiberTem::instance()
{
    static FiberTem inst;
    return inst;
}

sTemInfo FiberTem::getTemInfo()const
{
    return TemInst;
}

void FiberTem::clearInfo()
{
    TemInst.PN.clear();
    TemInst.FanoutPn.clear();
    TemInst.description.clear();

    // 重置 Fiber 数量
    TemInst.FiberCount = 0;

    // 重置 info 结构体中的字段为默认值
    TemInst.info.iface = InterfaceType::Unknown;       // 假设有 Unknown 或默认值
    TemInst.info.count = FiberCountType::Unknown;          // 默认 Fiber 数量
    TemInst.info.spec = FiberSpec::Unknown;           // 默认光纤规格
    TemInst.info.polarity = Polarity::Unknown;        // 默认极性
    TemInst.info.mode = FiberMode::Unknown;          // 默认模式
}

bool FiberTem::loadFromDatabase(const QString& pn)
{
    qDebug() << " 模拟从数据库加载模板号:" << pn;

    sTemInfo fakeInfo;
    fakeInfo.PN = pn;
    fakeInfo.FanoutPn = "123456789";
    fakeInfo.FiberCount = 2;

    fakeInfo.info.iface = InterfaceType::MTP_LC;
    fakeInfo.info.count = FiberCountType::F12;
    fakeInfo.info.spec = FiberSpec::OM4_Aqua;
    fakeInfo.info.polarity = Polarity::UNIVERSAL;
    fakeInfo.info.mode = FiberMode::SM;

    fakeInfo.description = "这是模拟的模板数据";
    TemInst = fakeInfo;

    return true;
}

// void FiberTem::setTemInfo(const TemInfo& info)
// {
//     TemInst = info;
// }
