// globals.cpp
#include "ip_BulkSend/globals.h"



//ip批量发送相关信息
QMap<QString, IP_sDataPacket<3>> tMap;  // 实际定义
QMap<QString, IP_sDataPacket<1>> sMap;
QMutex tMapMutex;                       // 定义互斥锁

QQueue<QJsonObject> TJsonQueue;  // 三相JSON队列
QMutex TQueueMutex;

QQueue<QJsonObject> SJsonQueue;  // 单相JSON队列
QMutex SQueueMutex;

int Stimesend;
int Ttimesend;

QMap<QString, QString>key_time;
