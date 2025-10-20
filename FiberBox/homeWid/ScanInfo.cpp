#include "ScanInfo.h"
#include <QDebug>
ScanInfo parseScanDataNewFormat(const QString &scanStr) {
    ScanInfo info;
    info.QRCodeContent = scanStr;

    QStringList fields = scanStr.split(";", Qt::SkipEmptyParts);
    if (fields.size() < 6) return info;

    info.id = fields[0].trimmed();
    info.serNum = fields[2].trimmed();
    info.wavelength = fields[3].trimmed();

    info.standard = fields[4].split(":").first().trimmed();    // "<0.5dB"
    QString str = info.standard;
    str.remove('<').remove("dB");      // "0.5"
    info.ilimit = str.toDouble();

    QStringList lines;
    for (int i = 5; i < fields.size(); ++i) {
        QString f = fields[i].trimmed();
        QStringList parts = f.split(":");
        if (parts.size() != 2) continue;
        QString fName = parts[0].trimmed();
        QString values = parts[1].trimmed();
        QStringList vals = values.split("/");
        if (vals.size() != 2) continue;
        lines << QString("%1: %2 / %3")
                     .arg(fName) // 直接用 F01、F02
                     .arg(vals[0].trimmed())
                     .arg(vals[1].trimmed());
    }

    info.rawData = lines.join("\n");
    info.count = lines.size();

    return info;
}

bool checkFiberLosses(const ScanInfo &info) {
    QStringList lines = info.rawData.split('\n', Qt::SkipEmptyParts);
    for (const QString &line : lines) {
        // line 格式: "F01: 0.23 / 0.21"
        QStringList parts = line.split(":");
        if (parts.size() != 2) continue;

        QString fiberName = parts[0].trimmed();
        QStringList values = parts[1].split("/");
        if (values.size() != 2) continue;

        double val1 = values[0].trimmed().toDouble();
        double val2 = values[1].trimmed().toDouble();

        if (val1 > info.ilimit || val2 > info.ilimit) {
            qDebug() << "[Mismatch] 光纤损耗超限!"
                     << fiberName
                     << "值1=" << val1
                     << "值2=" << val2
                     << "限值=" << info.ilimit;
            return true;  // 超标返回 true
        }
    }

    return false;  // 所有光纤都符合标准
}


QStringList reorderFibers(const QStringList &fibers, int count, const QString &polarity)
{
    QStringList result;

    if (polarity == "A" || polarity == "C") {
        // A/C 极性保持原顺序
        result = fibers;
    } else if (polarity == "Universal") {
        // Universal 按指定顺序重排
        QList<int> order;
        if (count == 8) {
            order = {0,7,1,6,2,5,3,4}; // 8芯
        } else if (count == 12) {
            order = {0,11,1,10,2,9,3,8,4,7,5,6}; // 12芯
        } else if (count == 16) {
            order = {0,15,1,14,2,13,3,12,4,11,5,10,6,9,7,8}; // 16芯
        } else {
            qWarning() << "[reorderFibers] 未知芯数，保持原顺序:" << count;
            return fibers;
        }

        for (int i : order) {
            if (i < fibers.size()) {
                result.append(fibers[i]);
            }
        }
    } else {
        qWarning() << "[reorderFibers] 未知极性，保持原顺序:" << polarity;
        result = fibers;
    }

    return result;
}
