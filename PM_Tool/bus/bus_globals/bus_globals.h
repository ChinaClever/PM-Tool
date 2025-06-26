// bus_globals.h
#ifndef BUS_GLOBALS_H
#define BUS_GLOBALS_H

#include "BusData.h"
#include <QVector>
#include <QReadWriteLock>
#include <QMap>
#include <QQueue>
#include <QJsonObject>
#include <QMutex>
extern QReadWriteLock busMapLock;

extern int cirCur;   // 回路电流
extern int boxNum;   //插接箱个数
extern int phaseVol; //全局电压
extern int outletPhase; //插接箱输出位数量
extern int cirNum;
extern QVector<QVector<QVector<double>>> phase; //相数据
extern QVector<QVector<double>> busPhase;  //始端箱相数据
extern QVector<QVector<QVector<double>>> outlet; //输出位数据

extern double cirEle[15][9];        //回路有功电能
extern double cirReacEle[15][9];    //无功电能

extern int bus_sendTime; //批量发送时间
extern QVector<QVector<double>>bulkPhase;
extern QMap<int,Busbar>busMap;
extern QQueue<QJsonObject>busQueue;
extern QMutex busBulkJQMutexes;
extern QMutex busBulkPhase;
extern int bulkBoxNum;
#endif // BUS_GLOBALS_H
