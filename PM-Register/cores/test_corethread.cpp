/*
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "test_corethread.h"
#include "baselogs.h"
Test_CoreThread::Test_CoreThread(QObject *parent) : BaseThread(parent)
{

}

void Test_CoreThread::workResult()
{
    BaseLogs *logs = BaseLogs::bulid();
    bool res = logs->setLogs(mDev->dt);

    qDebug()<<"res: "<<res;
}

void Test_CoreThread::run()
{
     if (isRun) return;
     isRun = true;


    // qDebug() << "user:" << mDev->dt.user
    //          << "keyLength:" << mDev->dt.keylength
    //          << "paddingMode:" << mDev->dt.paddingMode
    //          << "iv:" << mDev->dt.iv
    //          << "key:" << mDev->dt.key
    //          << "customer:" << mDev->dt.customer
    //          << "activationCode:" << mDev->dt.activationCode
    //          << "sn: " << mDev->dt.sn
    //          << "lis: " << mDev->dt.licenseFile;

    workResult();

    isRun = false;
}
