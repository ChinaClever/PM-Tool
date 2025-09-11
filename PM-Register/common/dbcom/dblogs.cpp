/*
 *
 *
 *  Created on: 2020年10月11日
 *      Author: Lzy
 */
#include "dblogs.h"

DbLogs::DbLogs()
{
    createTable();
    tableTile = tr("状态日志");
    //hiddens <<  9;
    headList << tr("用户") << tr("客户") << tr("到期时间") << tr("密钥长度") << tr("加密模式") << tr("填充方式") << tr("偏移量")
             << tr("密钥") << tr("序列号") << tr("激活码") << tr("许可文件");
}

void DbLogs::createTable()
{
    QString cmd =
        "CREATE TABLE IF NOT EXISTS %1 ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
        "date TEXT,"
        "time TEXT,"
        "user TEXT,"
        "customer TEXT,"
        "dateEnd TEXT,"
        "keyLength TEXT,"
        "encryptionMode TEXT,"
        "paddingMode TEXT,"
        "iv TEXT,"
        "key TEXT,"
        "sn TEXT,"
        "activationCode TEXT,"
        "licenseFile TEXT"
        ");";

    QSqlQuery query(mDb);
    if(!query.exec(cmd.arg(tableName()))) {
        throwError(query.lastError());
    }
}


DbLogs *DbLogs::bulid()
{
    static DbLogs* sington = nullptr;
    if(sington == nullptr)
        sington = new DbLogs();
    return sington;
}

bool DbLogs::insertItem(const sLogItem &item)
{
    QString cmd =
        "INSERT INTO %1 (date, time, user, customer, dateEnd, keyLength, encryptionMode, paddingMode, iv, key, sn, activationCode, licenseFile) "
        "VALUES (:date, :time, :user, :customer, :dateEnd, :keyLength, :encryptionMode, :paddingMode, :iv, :key, :sn, :activationCode, :licenseFile)";

    bool ret = modifyItem(item, cmd.arg(tableName()));
    if (ret) emit itemChanged(item.id, Insert);
    return ret;
}

bool DbLogs::modifyItem(const sLogItem &item, const QString &cmd)
{
    QSqlQuery query(mDb);
    query.prepare(cmd);

    query.bindValue(":date", item.date);
    query.bindValue(":time", item.time);
    query.bindValue(":user", item.user);
    query.bindValue(":customer", item.customer);
    query.bindValue(":dateEnd", item.dateEnd);
    query.bindValue(":keyLength", item.keylength);
    query.bindValue(":encryptionMode", item.encryption);
    query.bindValue(":paddingMode", item.paddingMode);
    query.bindValue(":iv", item.iv);
    query.bindValue(":key", item.key);
    query.bindValue(":sn", item.sn);
    query.bindValue(":activationCode", item.activationCode);
    query.bindValue(":licenseFile", item.licenseFile);

    bool ret = query.exec();
    if (!ret) throwError(query.lastError());
    return ret;
}

