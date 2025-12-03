#ifndef CONFIGBASH
#define CONFIGBASH
#include "cfgcom.h"



struct sCfgItem
{
    uchar addr;
    QString user;
    int logCount;
    uchar pcNum; //电脑号
    ushort currentNum;
    QString printIp;
};

class Cfg
{
    Cfg();
    ~Cfg();
public:
    static Cfg *bulid();

    sCfgItem *item;


    QString getLoginName();
    void setLoginName(const QString &name);

    QString getLanguage();
    void setLanguage(const QString& );

    void setCurrentNum();
    void write(const QString &key, const QVariant& v, const QString &g="cfg");
    QVariant read(const QString &key, const QVariant &v = QVariant(), const QString &g="cfg");

protected:
    bool getDate();
    void setDate();
    void initCurrentNum();

private:
    CfgCom *mCfg;
};

#endif // CONFIGBASH
