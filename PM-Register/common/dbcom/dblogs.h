#ifndef DBLOGS_H
#define DBLOGS_H
#include "basicsql.h"

struct sLogItem : public DbBasicItem{
    sLogItem(){}
    QString user, sn;
    QString customer;
    QString keylength;
    QString encryption;
    QString paddingMode;
    QString iv;
    QString key;
    QString activationCode;
    QString licenseFile;
    QString dateEnd;

};

class DbLogs : public SqlBasic<sLogItem>
{
    DbLogs();
public:
    static DbLogs* bulid();
    QString tableName(){return "logs";}
    bool insertItem(const sLogItem& item);

protected:
    void createTable();
    bool modifyItem(const sLogItem& item,const QString& cmd);
};

#endif // DBLOGS_H
