#ifndef FIBELTEM_H
#define FIBELTEM_H
#include <QString>
#include "fiberdefs.h"

class FiberTem
{
public:
    static FiberTem& instance();
    FiberTem(const FiberTem&) = delete;
    FiberTem& operator =(const FiberTem&) = delete;

    //void setTemInfo(const TemInfo& info);
    sTemInfo getTemInfo()const;

    bool loadFromDatabase(const QString& pn);
    void clearInfo();

private:
    FiberTem();
    ~FiberTem();

    sTemInfo TemInst;
    bool InstBeUsed; // 0 unused, 1 used
};

#endif // FIBELTEM_H
