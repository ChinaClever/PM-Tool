#pragma once

#include <QString>
#include <QSqlDatabase>
#include <QMutex>
class DatabaseManager {
public:
    static DatabaseManager& instance();

    bool init(const QString &dbPath = "db/energy_data.db");
    void writeLog(const QString &msg);
    void logError(const QString &msg);
    //ip三相电能
    bool createThreePhaseEnergyTable();
    bool insertOrUpdateThreePhaseEnergy(const QString &key, double v0, double v1, double v2);//更新三相电能
    bool queryThreePhaseEnergy(const QString &key, double &v0, double &v1, double &v2); //查找三相电能

    //ip单相电能
    bool createSignalPhaseEnergyTable();
    bool insertOrUpdateSignalPhaseEnergy(const QString &key, double v0);//更新三相电能
    bool querySignalPhaseEnergy(const QString &key, double &v0); //查找三相电能

    // 插接箱回路电能
    bool createBoxPhaseEnergyTable();
    bool insertOrUpdateBoxPhaseEnergy(const QString &key, const double eleActive[9], const double eleReactive[9]);
    bool queryBoxPhaseEnergy(const QString &key, double eleActive[9], double eleReactive[9]);

    // 最大48路 OutputBitEnergy
    bool createOutputBitEnergyTable();
    bool insertOrUpdateOutputBitEnergy(const QString &key, const double energy[48]);
    bool queryOutputBitEnergy(const QString &key, double energy[48]);

    QSqlDatabase database();

private:
    DatabaseManager() = default;
    ~DatabaseManager();

    QMutex m_dbMutex;  // 添加这个互斥锁成员
    QSqlDatabase m_db;
};
