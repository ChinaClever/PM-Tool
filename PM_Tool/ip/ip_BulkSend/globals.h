// globals.h
#pragma once
#include "ip_datapacket.h"
#include <QMutex>
#include <QQueue>       // 添加QQueue头文件
#include <QJsonObject>   // 添加QJsonObject头文件
#include <QJsonArray>    // 如需使用QJsonArray也加上

extern QMap<QString, IP_sDataPacket<3>> tMap;  // 声明为 extern
extern QMap<QString, IP_sDataPacket<1>> sMap;

extern QQueue<QJsonObject> TJsonQueue;  // 三相JSON队列
extern QMutex TQueueMutex;                  // 队列的互斥锁
