#include "fiberbasesql.h"

std::unique_ptr<QSqlDatabase> DatabaseManager::createConnection()
{
    auto db = std::make_unique<QSqlDatabase>();

    if (isSQLite()) {
        *db = QSqlDatabase::addDatabase("QSQLITE");
        db->setDatabaseName(m_sqlitePath);

        // 确保目录存在
        QFileInfo fileInfo(m_sqlitePath);
        QDir().mkpath(fileInfo.absolutePath());

        if (!db->open()) {
            qDebug() << "Database connection failed:" << db->lastError().text();
            return nullptr;
        }

        qDebug() << "SQLite database created at:" << m_sqlitePath;
    }

    return db;
}

FiberBaseSql::FiberBaseSql(QObject* parent)
    : QObject(parent)
{
    auto db = DatabaseManager::instance().createConnection();
    if (db) {
        m_db = *db;
    }
}

bool FiberBaseSql::remove(int id)
{
    return remove(QString("id = %1").arg(id));
}

bool FiberBaseSql::remove(const QString& condition)
{
    QSqlQuery query(m_db);
    const QString sql = QString("DELETE FROM %1 WHERE %2").arg(tableName(), condition);

    if (query.exec(sql)) {
        emit itemChanged(0, Operation::Remove);
        return true;
    }

    handleError(query.lastError());
    return false;
}

int FiberBaseSql::maxId() const
{
    QSqlQuery query(m_db);
    const QString sql = QString("SELECT MAX(id) FROM %1").arg(tableName());

    if (query.exec(sql) && query.next()) {
        return query.value(0).toInt();
    }

    return -1;
}

int FiberBaseSql::minId() const
{
    QSqlQuery query(m_db);
    const QString sql = QString("SELECT MIN(id) FROM %1").arg(tableName());

    if (query.exec(sql) && query.next()) {
        return query.value(0).toInt();
    }

    return -1;
}

int FiberBaseSql::count() const
{
    QSqlQuery query(m_db);
    const QString sql = QString("SELECT COUNT(*) FROM %1").arg(tableName());

    if (query.exec(sql) && query.next()) {
        return query.value(0).toInt();
    }

    return 0;
}

bool FiberBaseSql::clear()
{
    QSqlQuery query(m_db);
    const QString sql = QString("DELETE FROM %1").arg(tableName());

    if (query.exec(sql)) {
        emit itemChanged(0, Operation::Remove);
        return true;
    }

    handleError(query.lastError());
    return false;
}

// 修复：实现const版本的handleError
void FiberBaseSql::handleError(const QSqlError& error) const
{
    qDebug() << "Database Error:" << error.databaseText();
    qDebug() << "Driver Error:" << error.driverText();
    qDebug() << "Error Text:" << error.text();
}
