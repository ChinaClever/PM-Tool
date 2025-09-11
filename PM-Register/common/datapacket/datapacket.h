#ifndef DATAPACKET_H
#define DATAPACKET_H
#include <QColor>
#include <QtCore>
struct sDevInfo
{
    QString user;
    QString customer;
    QString keylength;
    QString encryption;
    QString paddingMode;
    QString iv;
    QString key;
    QString sn;
    QString activationCode;
    QString licenseFile;
    QString dateEnd;
};

struct sDevData
{
    sDevInfo dt; //设备类型
};

enum {
    Test_Fun, // 功能
    Test_Start, // 开始
    Test_Set,
    Test_Secure,
    Test_Collect,
    Test_raritan,
    Test_Over, // 终止
    Test_End, // 完成

    Test_Info=0,
    Test_Pass=1,
    Test_Fail=2,
};

struct sProgress
{
    sProgress() {step=0;}

    uchar step; // 步骤
    QString time;
    QList<bool> pass;
    QStringList status;

    uchar result;    // 最终结果
    QTime startTime;

    bool allTest;
};

class sDataPacket
{
    sDataPacket();
public:
    static sDataPacket *build();

    void init();
    sDevData *getDev() {return dev;}
    sProgress *getPro() {return pro;}

private:
    sDevData *dev;
    sProgress *pro;
};

#endif // DATAPACKET_H
