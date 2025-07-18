// globals.h
#pragma once
#include "ip_datapacket.h"
#include "mpro/data_struct/powersystemdata.h"
#include <QMutex>
#include <QQueue>       // 添加QQueue头文件
#include <QJsonObject>   // 添加QJsonObject头文件
#include <QJsonArray>    // 如需使用QJsonArray也加上



/* ip批量发送相关 */

extern QMap<QString, IP_sDataPacket<3>> tMap;  // 声明为 extern
extern QMap<QString, IP_sDataPacket<1>> sMap;

extern QQueue<QJsonObject> TJsonQueue;  // 三相JSON队列
extern QMutex TQueueMutex;                  // 队列的互斥锁

extern QQueue<QJsonObject> SJsonQueue;  // 单相JSON队列
extern QMutex SQueueMutex;

extern int Stimesend;
extern int Ttimesend;

extern QMap<QString, QString>key_time;

extern int SsendNum;
extern int TsendNum;

extern int SCnt;
extern int SCntt;
extern int SCntEr;
extern int TCnt;
extern int TCntt;
extern int TCntEr;



