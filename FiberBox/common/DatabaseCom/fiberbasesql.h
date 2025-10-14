#ifndef FIBERBASESQL_H
#define FIBERBASESQL_H

#include <QtSql>
#include <QObject>
#include <QDateTime>
#include <QDebug>
#include <memory>

class DatabaseTransaction
{
public:
    explicit DatabaseTransaction(QSqlDatabase& db) : m_db(db) {
        m_db.transaction();
    }

    ~DatabaseTransaction() {
        if (m_shouldRollback) {
            m_db.rollback();
        } else {
            m_db.commit();
        }
    }

    void rollback() { m_shouldRollback = true; }
    void commit() { m_shouldRollback = false; }

private:
    QSqlDatabase& m_db;
    bool m_shouldRollback = false;
};

// 基础数据结构
struct FiberBasicItem {
    uint id = 0;
    QString date;
    QString time;

    FiberBasicItem() {
        const auto now = QDateTime::currentDateTime();
        date = now.toString("yyyy-MM-dd");
        time = now.toString("hh:mm:ss");
    }
};

class DatabaseManager
{
public:
    enum class DatabaseType {
        SQLite,
        MySQL
    };

    // 单例模式
    static DatabaseManager& instance() {
        static DatabaseManager instance;
        return instance;
    }

    // 配置管理
    void setType(DatabaseType type) { m_type = type; }
    DatabaseType type() const { return m_type; }

    QString sqlitePath() const { return m_sqlitePath; }

    // MySQL配置
    void setMySQLConfig(const QString& host, int port, const QString& database,
                        const QString& username, const QString& password) {
        m_mysqlHost = host;
        m_mysqlPort = port;
        m_mysqlDatabase = database;
        m_mysqlUsername = username;
        m_mysqlPassword = password;
    }

    // 创建连接
    std::unique_ptr<QSqlDatabase> createConnection();

    // 类型判断
    bool isSQLite() const { return m_type == DatabaseType::SQLite; }
    bool isMySQL() const { return m_type == DatabaseType::MySQL; }

    // 设置数据库路径到db文件夹
    void setSQLitePath(const QString& path) {
        m_sqlitePath = path;
    }

    // 获取默认的db文件夹路径
    static QString getDefaultDbPath() {
        QDir appDir = QDir::current();
        QString dbDir = appDir.absoluteFilePath("db");

        // 确保db文件夹存在
        QDir().mkpath(dbDir);

        return QDir(dbDir).absoluteFilePath("fiberbox.db");
    }

    // 初始化数据库（创建文件夹和数据库文件）
    bool initializeDatabase() {
        if (isSQLite()) {
            QString dbPath = getDefaultDbPath();
            setSQLitePath(dbPath);
            qDebug() << "SQLite database path set to:" << dbPath;
            return true;
        }
        return false;
    }

private:
    DatabaseManager() = default;
    DatabaseType m_type = DatabaseType::SQLite;

    // SQLite配置
    QString m_sqlitePath = "fiberbox.db";

    // MySQL配置
    QString m_mysqlHost = "localhost";
    int m_mysqlPort = 3306;
    QString m_mysqlDatabase = "fiberbox";
    QString m_mysqlUsername = "root";
    QString m_mysqlPassword;
};

// 基础SQL类
class FiberBaseSql : public QObject
{
    Q_OBJECT

public:
    enum class Operation { Remove, Insert, Update };

    explicit FiberBaseSql(QObject* parent = nullptr);
    virtual ~FiberBaseSql(){};

    // 纯虚函数
    virtual QString tableName() const = 0;
    virtual void createTable() = 0;

    // 基础操作
    bool remove(int id);
    bool remove(const QString& condition);
    int maxId() const;
    int minId() const;
    int count() const;
    bool clear();

    QSqlDatabase& database() { return m_db; }
    const QSqlDatabase& database() const { return m_db; }

signals:
    void itemChanged(int id, Operation operation);

protected:

    void handleError(const QSqlError& error) const;

private:
    QSqlDatabase m_db;
};



#endif // FIBERBASESQL_H
