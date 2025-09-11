/*
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "baselogs.h"
extern QString user_land_name();

BaseLogs::BaseLogs(QObject *parent) : QThread(parent)
{

}

BaseLogs::~BaseLogs()
{
    wait();
}

BaseLogs *BaseLogs::bulid(QObject *parent)
{
    static BaseLogs* sington = nullptr;
    if(sington == nullptr)
        sington = new BaseLogs(parent);
    return sington;
}


bool BaseLogs::writeMac()
{
    sMacItem it;

    //it.fw = mDt->fwVersion;
    //it.user = mItem->user;
    //it.sn = mDt->sn;
    it.mac = mMac;
    mMac.clear();

    return DbMacs::bulid()->insertItem(it);
}

bool BaseLogs::setLogs(sDevInfo& dt)
{
    Db_Tran db;
    sLogItem it;

    it.activationCode = dt.activationCode;
    it.customer = dt.customer;
    it.encryption = dt.encryption;
    it.iv = dt.iv;
    it.key = dt.key;
    it.keylength = dt.keylength;
    it.licenseFile = dt.licenseFile;
    it.paddingMode = dt.paddingMode;
    it.sn = dt.sn;
    it.user = dt.user;
    it.dateEnd = dt.dateEnd;

    return DbLogs::bulid()->insertItem(it);
}

void BaseLogs::saveLogs()
{
    bool ret = writeLog();
    if(ret) {
        writeLogs();
        if(mMac.size()) writeMac();
    } else {
        // updatePro(tr("因未创建序列号，日志无法保存！"), false);
    }
    mLogItems.clear();
}

bool BaseLogs::writeLog()
{
    Db_Tran db;
    sLogItem it;



    return DbLogs::bulid()->insertItem(it);
}

bool BaseLogs::initItem(sStateItem &it)
{


    return it.sn.size();
}

void BaseLogs::writeLogs()
{
    Db_Tran db;
    for(int i=0; i<mLogItems.size(); ++i) {
        sStateItem it = mLogItems.at(i);
        if(initItem(it)) DbStates::bulid()->insertItem(it);
    }
    mLogItems.clear();
}
