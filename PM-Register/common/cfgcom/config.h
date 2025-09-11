#ifndef CONFIGBASH
#define CONFIGBASH
#include "cfgcom.h"

/**
 * RTU传输统计结构体
 */
struct sCount
{
    sCount() {all=ok=err=cnt=0;}

    int cnt;
    int all;
    int ok;
    int err;
};

struct sMac
{
    sMac() {cntMac=0;}
    int cntMac;
    QString mac;
    QString user;
    QString startMac;
    QString endMac;
};

struct sCfgItem
{
    uchar addr;
    sMac macs;
    QString user;
    sCount cnt;
    int logCount;
    uchar aiMode;
    uchar pcNum;
    ushort currentNum;


};


class Cfg
{
    Cfg();
    ~Cfg();
public:
    static Cfg *bulid();

    sCfgItem *item;

    QString getSerialBr(const QString &com);
    QString getSerialName(const QString &key);
    void setSerialBr(const QString &com, const QString &br);
    void setSerialName(const QString &key, const QString &v);

    QString getLoginName();
    void setLoginName(const QString &name);

    void wirteMac();
    void writeCnt();
    void writeCfgDev();

    void setCurrentNum();
    void write(const QString &key, const QVariant& v, const QString &g="cfg");
    QVariant read(const QString &key, const QVariant &v = QVariant(), const QString &g="cfg");

protected:
    void initMac();
    void initCnt();
    bool getDate();
    void setDate();
    void initCfgDev();
    void initCurrentNum();

private:
    CfgCom *mCfg;
};

#endif // CONFIGBASH
