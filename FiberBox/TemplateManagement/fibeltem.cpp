#include "fibeltem.h"
#include "dbtem.h"
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
    TemInst.FanCount = 0;

    // 重置 info 结构体中的字段为默认值
    TemInst.info.iface = InterfaceType::Unknown;       // 假设有 Unknown 或默认值
    TemInst.info.count = FiberCountType::Unknown;          // 默认 Fiber 数量
    TemInst.info.spec = FiberSpec::Unknown;           // 默认光纤规格
    TemInst.info.polarity = Polarity::Unknown;        // 默认极性
    TemInst.info.mode = FiberMode::Unknown;          // 默认模式
}

bool FiberTem::loadFromDatabase(const QString& pn)
{
    qDebug() << "加载模板号:" << pn;

    auto db = DbTem::build();
    sTemItem item = db->findByPN(pn);

    if (item.data.PN.isEmpty()) {
        qWarning() << "未找到模板:" << pn;
        return false;
    }

    TemInst = item.data;
    qDebug() << "加载模板成功:" << TemInst.PN
             << " 描述:" << TemInst.description
             << " 模式:" << (int)TemInst.info.mode;
    return true;
}



// void FiberTem::setTemInfo(const TemInfo& info)
// {
//     TemInst = info;
// }
