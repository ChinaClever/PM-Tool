#ifndef TEMPLATESQL_H
#define TEMPLATESQL_H

#include "fiberbasesql.h"
#include "TemplateManagement/fiberdefs.h"

class TemplateSql : public FiberBaseSql
{
public:
    explicit TemplateSql(QObject* parent = nullptr);

    QString tableName() const override;
    void createTable() override;

    // 写
    bool upsert(const sTemInfo& info);      // 存在则覆盖（删后插）
    bool insertOne(const sTemInfo& info);

    // 读
    bool findByPN(const QString& pn, sTemInfo& out);
    QVector<sTemInfo> findAll();

private:
    bool exists(const QString& pn);
    sTemInfo rowToTemInfo(QSqlQuery& q) const;
};

#endif // TEMPLATESQL_H
