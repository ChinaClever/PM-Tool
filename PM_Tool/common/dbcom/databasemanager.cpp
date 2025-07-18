#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMutexLocker>

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::~DatabaseManager() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DatabaseManager::init(const QString &dbPath) {
    if (QSqlDatabase::contains("AppConnection")) {
        m_db = QSqlDatabase::database("AppConnection");
    } else {
        m_db = QSqlDatabase::addDatabase("QSQLITE", "AppConnection");
        m_db.setDatabaseName(dbPath);
    }

    if (!m_db.open()) {
        qDebug() << "❌ Failed to open DB:" << m_db.lastError().text();
        return false;
    }

    QSqlQuery query(m_db);


    if (!query.exec("PRAGMA journal_mode=WAL;"))
        qDebug() << "⚠️ 设置 WAL 模式失败:" << query.lastError().text();
    else
        qDebug() << "✅ WAL 模式已启用";

    if (!query.exec("PRAGMA synchronous=NORMAL;"))
        qDebug() << "⚠️ 设置同步模式失败:" << query.lastError().text();
    else
        qDebug() << "✅ 同步模式已设置为 NORMAL";

     m_db.transaction();

    if (!query.exec("PRAGMA cache_size=-8192;"))
        qDebug() << "⚠️ 设置缓存大小失败:" << query.lastError().text();
    else
        qDebug() << "✅ 缓存大小已设置为 8MB";

    if (!query.exec("PRAGMA temp_store=MEMORY;"))
        qDebug() << "⚠️ 设置临时存储模式失败:" << query.lastError().text();
    else
        qDebug() << "✅ 临时存储模式已设置为 MEMORY";

    m_db.commit();
    qDebug() << "✅ Database opened at:" << dbPath;
    return true;
}

// --------------------------------------------
// 各个表的创建和操作函数全部加锁
// --------------------------------------------

bool DatabaseManager::createSignalPhaseEnergyTable() {
    QMutexLocker locker(&m_dbMutex);

    if (!m_db.isOpen()) {
        qDebug() << "Database not open!";
        return false;
    }

    QSqlQuery query(m_db);
    const char *sql = R"(
        CREATE TABLE IF NOT EXISTS SignalPhaseEnergy (
            key TEXT PRIMARY KEY,
            ele_active REAL
        )
    )";

    if (!query.exec(sql)) {
        qDebug() << "Create SignalPhaseEnergy table failed:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::insertOrUpdateSignalPhaseEnergy(const QString &key, double v0) {
    QMutexLocker locker(&m_dbMutex);

    if (!m_db.isOpen()) {
        qDebug() << "Database not open!";
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(R"(
        UPDATE SignalPhaseEnergy SET ele_active = :v0 WHERE key = :key
    )");
    query.bindValue(":key", key);
    query.bindValue(":v0", v0);

    if (!query.exec()) {
        qDebug() << "Update SignalPhaseEnergy failed:" << query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() == 0) {
        query.prepare(R"(
            INSERT INTO SignalPhaseEnergy (key, ele_active) VALUES (:key, :v0)
        )");
        query.bindValue(":key", key);
        query.bindValue(":v0", v0);

        if (!query.exec()) {
            qDebug() << "Insert SignalPhaseEnergy failed:" << query.lastError().text();
            return false;
        }
    }
    return true;
}

bool DatabaseManager::querySignalPhaseEnergy(const QString &key, double &v0) {
    QMutexLocker locker(&m_dbMutex);

    if (!m_db.isOpen()) {
        qDebug() << "Database not open!";
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(R"(
        SELECT ele_active FROM SignalPhaseEnergy WHERE key = :key
    )");
    query.bindValue(":key", key);

    if (!query.exec()) {
        qDebug() << "Query SignalPhaseEnergy failed:" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        v0 = query.value(0).toDouble();
        return true;
    }
    return false;
}

// 三相电能表类似，全部加锁
bool DatabaseManager::createThreePhaseEnergyTable() {
    QMutexLocker locker(&m_dbMutex);

    if (!m_db.isOpen()) {
        qDebug() << "Database not open!";
        return false;
    }

    QSqlQuery query(m_db);
    const char *sql = R"(
        CREATE TABLE IF NOT EXISTS ThreePhaseEnergy (
            key TEXT PRIMARY KEY,
            ele_active_0 REAL,
            ele_active_1 REAL,
            ele_active_2 REAL
        )
    )";

    if (!query.exec(sql)) {
        qDebug() << "Create ThreePhaseEnergy table failed:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::insertOrUpdateThreePhaseEnergy(const QString &key, double v0, double v1, double v2) {
    QMutexLocker locker(&m_dbMutex);

    if (!m_db.isOpen()) {
        qDebug() << "Database not open!";
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(R"(
        INSERT INTO ThreePhaseEnergy (key, ele_active_0, ele_active_1, ele_active_2)
        VALUES (:key, :v0, :v1, :v2)
        ON CONFLICT(key) DO UPDATE SET
            ele_active_0=excluded.ele_active_0,
            ele_active_1=excluded.ele_active_1,
            ele_active_2=excluded.ele_active_2
    )");
    query.bindValue(":key", key);
    query.bindValue(":v0", v0);
    query.bindValue(":v1", v1);
    query.bindValue(":v2", v2);

    if (!query.exec()) {
        qDebug() << "Insert/update ThreePhaseEnergy failed:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::queryThreePhaseEnergy(const QString &key, double &v0, double &v1, double &v2) {
    QMutexLocker locker(&m_dbMutex);

    if (!m_db.isOpen()) {
        qDebug() << "Database not open!";
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT ele_active_0, ele_active_1, ele_active_2 FROM ThreePhaseEnergy WHERE key = :key");
    query.bindValue(":key", key);

    if (!query.exec()) {
        qDebug() << "Query ThreePhaseEnergy failed:" << query.lastError().text();
        return false;
    }
    if (query.next()) {
        v0 = query.value(0).toDouble();
        v1 = query.value(1).toDouble();
        v2 = query.value(2).toDouble();
        return true;
    }
    return false;
}

// 6相电能表操作
bool DatabaseManager::createBoxPhaseEnergyTable() {
    QMutexLocker locker(&m_dbMutex);

    if (!m_db.isOpen()) {
        qDebug() << "Database not open!";
        return false;
    }

    QSqlQuery query(m_db);
    const char *sql = R"(
        CREATE TABLE IF NOT EXISTS BoxPhaseEnergy (
            key TEXT PRIMARY KEY,
            ele_active_0 REAL,
            ele_active_1 REAL,
            ele_active_2 REAL,
            ele_active_3 REAL,
            ele_active_4 REAL,
            ele_active_5 REAL,
            ele_reactive_0 REAL,
            ele_reactive_1 REAL,
            ele_reactive_2 REAL,
            ele_reactive_3 REAL,
            ele_reactive_4 REAL,
            ele_reactive_5 REAL
        )
    )";

    if (!query.exec(sql)) {
        qDebug() << "Create BoxPhaseEnergy table failed:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::insertOrUpdateBoxPhaseEnergy(const QString &key, const double eleActive[6], const double eleReactive[6]) {
    QMutexLocker locker(&m_dbMutex);

    if (!m_db.isOpen()) {
        qDebug() << "Database not open!";
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(R"(
        INSERT INTO BoxPhaseEnergy (
            key,
            ele_active_0, ele_active_1, ele_active_2, ele_active_3, ele_active_4, ele_active_5,
            ele_reactive_0, ele_reactive_1, ele_reactive_2, ele_reactive_3, ele_reactive_4, ele_reactive_5
        ) VALUES (
            :key,
            :ea0, :ea1, :ea2, :ea3, :ea4, :ea5,
            :er0, :er1, :er2, :er3, :er4, :er5
        )
        ON CONFLICT(key) DO UPDATE SET
            ele_active_0=excluded.ele_active_0,
            ele_active_1=excluded.ele_active_1,
            ele_active_2=excluded.ele_active_2,
            ele_active_3=excluded.ele_active_3,
            ele_active_4=excluded.ele_active_4,
            ele_active_5=excluded.ele_active_5,
            ele_reactive_0=excluded.ele_reactive_0,
            ele_reactive_1=excluded.ele_reactive_1,
            ele_reactive_2=excluded.ele_reactive_2,
            ele_reactive_3=excluded.ele_reactive_3,
            ele_reactive_4=excluded.ele_reactive_4,
            ele_reactive_5=excluded.ele_reactive_5
    )");

    query.bindValue(":key", key);
    for (int i = 0; i < 6; ++i) {
        query.bindValue(QString(":ea%1").arg(i), eleActive[i]);
        query.bindValue(QString(":er%1").arg(i), eleReactive[i]);
    }

    if (!query.exec()) {
        qDebug() << "Insert/update BoxPhaseEnergy failed:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::queryBoxPhaseEnergy(const QString &key, double eleActive[6], double eleReactive[6]) {
    QMutexLocker locker(&m_dbMutex);

    if (!m_db.isOpen()) {
        qDebug() << "Database not open!";
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(R"(
        SELECT
            ele_active_0, ele_active_1, ele_active_2, ele_active_3, ele_active_4, ele_active_5,
            ele_reactive_0, ele_reactive_1, ele_reactive_2, ele_reactive_3, ele_reactive_4, ele_reactive_5
        FROM BoxPhaseEnergy WHERE key = :key
    )");
    query.bindValue(":key", key);

    if (!query.exec()) {
        qDebug() << "Query BoxPhaseEnergy failed:" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        for (int i = 0; i < 6; ++i) {
            eleActive[i] = query.value(i).toDouble();
            eleReactive[i] = query.value(i + 6).toDouble();
        }
        return true;
    }
    return false;
}

// 48路 OutputBitEnergy 操作
bool DatabaseManager::createOutputBitEnergyTable() {
    QMutexLocker locker(&m_dbMutex);

    if (!m_db.isOpen()) {
        qDebug() << "Database not open!";
        return false;
    }

    QString sql = "CREATE TABLE IF NOT EXISTS OutputBitEnergy (key TEXT PRIMARY KEY";
    for (int i = 0; i < 48; i++) {
        sql += QString(", energy_%1 REAL").arg(i);
    }
    sql += ");";

    QSqlQuery query(m_db);
    if (!query.exec(sql)) {
        qDebug() << "❌ Create OutputBitEnergy table failed:" << query.lastError().text();
        return false;
    }
    qDebug() << "✅ OutputBitEnergy table ready";
    return true;
}

bool DatabaseManager::insertOrUpdateOutputBitEnergy(const QString &key, const double energies[48]) {
    QMutexLocker locker(&m_dbMutex);

    if (!m_db.isOpen()) {
        qDebug() << "Database not open!";
        return false;
    }
    if (energies == nullptr) {
        qDebug() << "Energies array is null!";
        return false;
    }

    QSqlQuery query(m_db);

    QString updateSql = "UPDATE OutputBitEnergy SET ";
    for (int i = 0; i < 48; ++i) {
        updateSql += QString("energy_%1 = :energy_%1").arg(i);
        if (i != 47) updateSql += ", ";
    }
    updateSql += " WHERE key = :key";

    query.prepare(updateSql);
    query.bindValue(":key", key);
    for (int i = 0; i < 48; ++i) {
        query.bindValue(QString(":energy_%1").arg(i), energies[i]);
    }

    if (!query.exec()) {
        qDebug() << "Update OutputBitEnergy failed:" << query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() == 0) {
        QString insertSql = "INSERT INTO OutputBitEnergy (key";
        QString valuesSql = " VALUES (:key";
        for (int i = 0; i < 48; ++i) {
            insertSql += QString(", energy_%1").arg(i);
            valuesSql += QString(", :energy_%1").arg(i);
        }
        insertSql += ")";
        valuesSql += ")";
        QString fullInsertSql = insertSql + valuesSql;

        query.prepare(fullInsertSql);
        query.bindValue(":key", key);
        for (int i = 0; i < 48; ++i) {
            query.bindValue(QString(":energy_%1").arg(i), energies[i]);
        }

        if (!query.exec()) {
            qDebug() << "Insert OutputBitEnergy failed:" << query.lastError().text();
            return false;
        }
    }
    return true;
}

bool DatabaseManager::queryOutputBitEnergy(const QString &key, double energies[48]) {
    QMutexLocker locker(&m_dbMutex);

    if (!m_db.isOpen()) {
        qDebug() << "Database not open!";
        return false;
    }

    QSqlQuery query(m_db);
    QString sql = "SELECT ";
    for (int i = 0; i < 48; ++i) {
        sql += QString("energy_%1").arg(i);
        if (i != 47) sql += ", ";
    }
    sql += " FROM OutputBitEnergy WHERE key = :key";

    query.prepare(sql);
    query.bindValue(":key", key);

    if (!query.exec()) {
        qDebug() << "Query OutputBitEnergy failed:" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        for (int i = 0; i < 48; ++i) {
            energies[i] = query.value(i).toDouble();
        }
        return true;
    }
    return false;
}

QSqlDatabase DatabaseManager::database() {
    return m_db;
}
