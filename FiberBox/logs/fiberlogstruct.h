#ifndef FIBERLOGSTRUCT_H
#define FIBERLOGSTRUCT_H

#include "basicsql.h"  //
// 日志表结构体（继承通用的数据库项）
struct sFiberLogItem : public DbBasicItem
{
    QString boxId;         // 光纤盒ID
    QString PN;            // 成品编号
    QString fanoutPn;      // 扇出线PN
    QString description;   // 描述
    QString fanoutId;      // 扇出线id
    QString waveType;      // 波类型
    double limitIL = 0.0;  // 最大限值IL
    QString qrContent;     // 二维码内容

    // 扇出线序列号和二维码内容
    QString seq1, qr1;
    QString seq2, qr2;
    QString seq3, qr3;
    QString seq4, qr4;
};

#endif // FIBERLOGSTRUCT_H
