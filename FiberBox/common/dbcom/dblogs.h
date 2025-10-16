#ifndef DBLOGS_H
#define DBLOGS_H

#include "basicsql.h"
#include "fiberlogstruct.h"

class DbLogs : public SqlBasic<sFiberLogItem>
{
public:
    static DbLogs* build();

    DbLogs();

    QString tableName() override { return "logs"; }
    void createTable() override;

    bool insertItem(const sFiberLogItem &item);

protected:
    void selectItem(QSqlQuery &query, sFiberLogItem &item) override;
};

#endif // DBLOGS_H
