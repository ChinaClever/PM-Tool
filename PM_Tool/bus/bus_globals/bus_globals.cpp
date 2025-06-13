#include "bus_globals.h"

int cirCur = 63;
int phaseVol = 230;
int outNum = 3;
double cirEle[15][9] = {0};
double cirReacEle[15][9] = {0};

QVector<QVector<QVector<double>>> phase(
    15,
    QVector<QVector<double>>(
        3,
        QVector<double>(9, 0.0)
        )
    );
