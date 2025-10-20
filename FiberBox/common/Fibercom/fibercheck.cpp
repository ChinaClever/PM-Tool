#include "fibercheck.h"
#include <QDebug>
FiberCheck::FiberCheck() {}

bool FiberCheck::verify(const QString& pn)
{
    QString code = pn.trimmed();

    if (code.size() < 6) {
        qWarning() << "检测失败：模板号长度不足6位 ->" << code;
        return false;
    }

    if (!code.startsWith("2600")) {
        qWarning() << "检测失败：模板号不是2600开头 ->" << code;
        return false;
    }

    qDebug() << "检测通过：" << code;
    return true;
}


