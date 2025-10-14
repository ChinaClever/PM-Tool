// 新建文件：databaseinitializer.h
#ifndef DATABASEINITIALIZER_H
#define DATABASEINITIALIZER_H

#include "fiberbasesql.h"
#include "templatesql.h"
#include <QObject>

class DatabaseInitializer : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseInitializer(QObject *parent = nullptr);

    // 初始化数据库
    bool initialize();

    // 创建所有表
    bool createAllTables();

private:
    // bool createLogTable();
    bool createTemplateTable();
    // bool createUserTable();

    // LogSql* m_logSql;
     TemplateSql* m_templateSql;
    // UserSql* m_userSql;
};

#endif // DATABASEINITIALIZER_H
