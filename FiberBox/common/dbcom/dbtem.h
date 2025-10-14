#ifndef DBTEM_H
#define DBTEM_H

#include "basicsql.h"
#include "fiberdefs.h"

// 模板数据项
struct sTemItem : public DbBasicItem
{
    sTemInfo data;
    sTemItem() {}
};

class DbTem : public SqlBasic<sTemItem>
{
    DbTem(); // 构造私有，单例

public:
    static DbTem* build(); // 获取单例

    QString tableName() override { return "templates"; }

    bool insertItem(sTemItem &item);
    bool updateItem(const sTemItem &item);
    void removeItemByPN(const QString& pn);
    bool getItemByPN(const QString& pn, sTemItem &item);

protected:
    void createTable() override;
    bool modifyItem(const sTemItem& item, const QString& cmd);
    void selectItem(QSqlQuery &query, sTemItem &item);
};

#endif // DBTEM_H
