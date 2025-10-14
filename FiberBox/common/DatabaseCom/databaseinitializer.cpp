// 新建文件：databaseinitializer.cpp
#include "databaseinitializer.h"
#include <QDir>
#include <QStandardPaths>

DatabaseInitializer::DatabaseInitializer(QObject *parent)
    : QObject(parent)
    // , m_logSql(nullptr)
     , m_templateSql(nullptr)
    // , m_userSql(nullptr)
{

}

bool DatabaseInitializer::initialize()
{
    auto& dbManager = DatabaseManager::instance();
    dbManager.setType(DatabaseManager::DatabaseType::SQLite);

    if (!dbManager.initializeDatabase()) {
        qDebug() << "Failed to initialize database";
        return false;
    }

    // 关键：显式打开一次连接，确保 SQLite 文件和连接有效
    auto testDb = dbManager.createConnection();
    if (!testDb) {
        qDebug() << "Database open failed, abort creating tables";
        return false;
    }

    m_templateSql = new TemplateSql(this);
    return createAllTables();
}

bool DatabaseInitializer::createAllTables()
{
    bool success = true;

    // // 创建日志表
    // if (!createLogTable()) {
    //     qDebug() << "Failed to create log table";
    //     success = false;
    // }

    // 创建模板表
    if (!createTemplateTable()) {
        qDebug() << "Failed to create template table";
        success = false;
    }

    // // 创建用户表
    // if (!createUserTable()) {
    //     qDebug() << "Failed to create user table";
    //     success = false;
    // }

    if (success) {
        qDebug() << "All database tables created successfully";
    }

    return success;
}

bool DatabaseInitializer::createTemplateTable()
{
    try {
        m_templateSql->createTable();
        qDebug() << "Template table created successfully";
        return true;
    } catch (...) {
        return false;
    }
}


