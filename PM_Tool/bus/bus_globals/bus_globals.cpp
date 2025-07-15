#include "bus_globals.h"
#include "qjsonobject.h"

int cirCur = 63;
int phaseVol = 230;
int boxNum = 9;
int outletPhase = 3; //插接箱输出位数量
int cirNum = 3; //插接箱回路数量
double cirEle[15][9];
double cirReacEle[15][9];

int bus_sendTime;


QVector<QVector<QVector<double>>> phase(
    15,
    QVector<QVector<double>>(
        3,
        QVector<double>(9, 0.0)
        )
    );

QVector<QVector<double>>busPhase(10,QVector<double>(3,0));

QVector<QVector<QVector<double>>> outlet(
    15,
    QVector<QVector<double>>(
        3,
        QVector<double>(7, 0.0)
        )
    );

//批量处理相关
QVector<QVector<double>>bulkPhase(6,QVector<double>(3,0.0)); //回路相数据保存
QMap<int,Busbar>busMap;
QQueue<QJsonObject>busQueue;
QMutex busBulkJQMutexes; //批量进队锁
QMutex busBulkPhase; //批量进队锁
QReadWriteLock busMapLock;
int bulkBoxNum;
int bulkBusNum;

int BusCnt;
int BusCntt;
int BusCntEr;
