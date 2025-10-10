#ifndef FIBELTEM_H
#define FIBELTEM_H
#include <QString>
#include "fiberdefs.h"
struct TemInfo{
    QString PN;
    QString FanoutPn;
    int FiberCount;
    FiberInfo info;
    QString description;
};

class FiberTem
{
public:
    static FiberTem& instance();
    FiberTem(const FiberTem&) = delete;
    FiberTem& operator =(const FiberTem&) = delete;

    //void setTemInfo(const TemInfo& info);
    TemInfo getTemInfo()const;

    bool loadFromDatabase(const QString& pn);
    void clearInfo();

private:
    FiberTem();
    ~FiberTem();

    TemInfo TemInst;
    bool InstBeUsed; // 0 unused, 1 used
};

#endif // FIBELTEM_H
