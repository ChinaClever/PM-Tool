#ifndef TEST_CORETHREAD_H
#define TEST_CORETHREAD_H
#include "baseobject.h"


class Test_CoreThread : public BaseThread
{
    Q_OBJECT
public:
    explicit Test_CoreThread(QObject *parent = nullptr);

    bool isContinue;

    void setFlag(bool x){flag = x;}
signals:
    void waitSig();
    void updateLcd(const QString &message);

protected:
    void run();
    void workResult();

private:
    bool flag;

};

#endif // TEST_CORETHREAD_H
