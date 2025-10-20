#ifndef SCANINFO_H
#define SCANINFO_H

#include <QString>
#include <QStringList>

struct ScanInfo {
    QString id;
    QString serNum;
    int count;
    QString wavelength;
    double ilimit;
    QString standard;
    QString rawData;
    QString QRCodeContent;
};

// 只声明函数，不写实现
ScanInfo parseScanDataNewFormat(const QString &scanStr);
bool checkFiberLosses(const ScanInfo &info);

#endif // SCANINFO_H
