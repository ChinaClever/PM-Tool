// bus_globals.h
#ifndef BUS_GLOBALS_H
#define BUS_GLOBALS_H
#include <QVector>
extern int cirCur;  // 只声明，不定义
extern int outNum;
extern int phaseVol;

extern QVector<QVector<QVector<double>>> phase;


extern double cirEle[15][9];
extern double cirReacEle[15][9];
#endif // BUS_GLOBALS_H
