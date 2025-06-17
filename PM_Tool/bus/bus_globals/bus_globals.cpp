#include "bus_globals.h"

int cirCur = 63;
int phaseVol = 230;
int boxNum = 3;
int outletPhase = 3; //插接箱输出位数量
int cirNum = 3; //插接箱回路数量
double cirEle[15][9] = {0};
double cirReacEle[15][9] = {0};

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
