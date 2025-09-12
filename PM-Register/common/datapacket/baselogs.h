#ifndef BASELOGS_H
#define BASELOGS_H
#include "dblogs.h"
#include "baseobject.h"

class BaseLogs : public QThread, public BaseObject
{
    Q_OBJECT
    explicit BaseLogs(QObject *parent = nullptr);
public:
    static BaseLogs *bulid(QObject *parent = nullptr);
    ~BaseLogs();

    void saveLogs();
    bool setLogs(sDevInfo &);
    void writeMac(const QString &mac){mMac=mac;}

protected:
    void run(){saveLogs();}
    bool writeLog();
private:
    QString mMac;
};

#endif // BASELOGS_H
