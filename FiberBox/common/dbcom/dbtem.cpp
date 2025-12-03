#include "dbtem.h"
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>

DbTem::DbTem()
{
    createTable();
    tableTile = QObject::tr("模板管理");
    hiddens << 1 << 2;
    headList << QObject::tr("成品编号") << QObject::tr("描述") << QObject::tr("极性")
             << QObject::tr("类型") << QObject::tr("QR模板") << QObject::tr("Fanout PN")
             << QObject::tr("用量") << QObject::tr("标贴模板") << QObject::tr("λ1")
             << QObject::tr("λ2") << QObject::tr("IL 限值") << QObject::tr("类型SM/MM") << QObject::tr("光纤规格");
}


DbTem* DbTem::build()
{
    static DbTem* instance = nullptr;
    if(!instance) instance = new DbTem();

    return instance;
}

void DbTem::createTable()
{
    QString cmd =
        "CREATE TABLE IF NOT EXISTS %1 ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
        "date           TEXT,"
        "time           TEXT,"
        "PN TEXT NOT NULL UNIQUE,"
        "description TEXT,"
        "polarity TEXT,"
        "iface TEXT,"
        "qrTemplate TEXT,"
        "FanoutPn TEXT,"
        "FanCount INTEGER DEFAULT 1,"
        "labelTemplate TEXT,"
        "lambda1 INTEGER,"
        "lambda2 INTEGER,"
        "limit_value REAL DEFAULT 0.0,"
        "mode TEXT,"
        "spec TEXT"
        ");";
    QSqlQuery query(mDb);
    if(!query.exec(cmd.arg(tableName()))) {
        throwError(query.lastError());
    }
}

bool DbTem::insertItem(sTemItem &item)
{
    item.id = maxId() + 1;

    QString cmd = "INSERT INTO %1 (id, date, time, PN, description, polarity, iface, qrTemplate, "
                  "FanoutPn, FanCount, labelTemplate, lambda1, lambda2, limit_value, mode, spec) "
                  "VALUES (:id,:date,:time,:PN,:description,:polarity,:iface,:qrTemplate,:FanoutPn,"
                  ":FanCount,:labelTemplate,:lambda1,:lambda2,:limit_value,:mode,:spec)";


    bool ret = modifyItem(item, cmd.arg(tableName()));
    if(ret) emit itemChanged(item.id, Insert);
    return ret;
}

bool DbTem::updateItem(const sTemItem &item)
{
    QString cmd = "UPDATE %1 SET date=:date, time=:time, PN=:PN, description=:description, polarity=:polarity, "
                  "iface=:iface, qrTemplate=:qrTemplate, FanoutPn=:FanoutPn, FanCount=:FanCount, "
                  "labelTemplate=:labelTemplate, lambda1=:lambda1, lambda2=:lambda2, "
                  "limit_value=:limit_value, mode=:mode, spec=:spec "
                  "WHERE id=:id";

    bool ret = modifyItem(item, cmd.arg(tableName()));
    if(ret) emit itemChanged(item.id, Update);
    return ret;
}

QVector<sTemItem> DbTem::getItemsByPN(const QString &pn)
{
    return selectItems(QString("WHERE PN='%1'").arg(pn));
}

sTemItem DbTem::findByPN(const QString &pn)
{
    auto list = getItemsByPN(pn);
    if (!list.isEmpty())
        return list.first();
    return sTemItem();
}

bool DbTem::getItemByPN(const QString& pn, sTemItem &item)
{
    QString cmd = QString("SELECT *FROM %1 WHERE PN = :PN").arg(tableName());
    QSqlQuery query(mDb);
    query.prepare(cmd);
    query.bindValue(":PN", pn);

    if(!query.exec()){
        throwError(query.lastError());
        return false;
    }
    if(query.next()){
        selectItem(query, item);
        qDebug() << " 找到模板 PN:" << pn;
        return true;
    } else {
        qDebug() << " 未找到模板 PN:" << pn;
        return false;
    }

}

void DbTem::removeItemByPN(const QString& pn)
{
    QString cmd = QString("DELETE FROM %1 WHERE PN=:PN").arg(tableName());
    QSqlQuery query(mDb);
    query.prepare(cmd);
    query.bindValue(":PN", pn);
    if(!query.exec()) {
        throwError(query.lastError());
    }
}

bool DbTem::modifyItem(const sTemItem &item, const QString &cmd)
{
    QSqlQuery query(mDb);
    query.prepare(cmd);

    query.bindValue(":id", item.id);
    query.bindValue(":date", item.date);
    query.bindValue(":time", item.time);
    query.bindValue(":PN", item.data.PN);
    query.bindValue(":description", item.data.description);
    query.bindValue(":polarity", polarityToString(item.data.info.polarity));

    QString ifaceCountStr = ifaceToString(item.data.info.iface) + " " + countToString(item.data.info.count);
    query.bindValue(":iface", ifaceCountStr);

    query.bindValue(":qrTemplate", item.data.qrTemplate);
    query.bindValue(":FanoutPn", item.data.FanoutPn);
    query.bindValue(":FanCount", item.data.FanCount);
    query.bindValue(":labelTemplate", item.data.labelTemplate);
    query.bindValue(":lambda1", item.data.lambda.first);
    query.bindValue(":lambda2", item.data.lambda.second);
    query.bindValue(":limit_value", item.data.limit);
    query.bindValue(":mode", modeToString(item.data.info.mode));
    query.bindValue(":spec", specToString(item.data.info.spec));


    bool ret = query.exec();
    if(!ret) throwError(query.lastError());
    return ret;
}

void DbTem::selectItem(QSqlQuery &query, sTemItem &item)
{
    item.id = query.value("id").toInt();
    QString dateStr = query.value("date").toString();
    QString timeStr = query.value("time").toString();

    item.data.PN            = query.value("PN").toString();
    item.data.description   = query.value("description").toString();
    item.data.FanoutPn      = query.value("FanoutPn").toString();
    item.data.FanCount      = query.value("FanCount").toInt();
    item.data.limit         = query.value("limit_value").toDouble();

    QString ifaceCountStr = query.value("iface").toString(); // "MTP-LC 16F"
    QStringList parts = ifaceCountStr.split(' ');
    if(parts.size() >= 2){
        item.data.info.iface = stringToIface(parts[0]);
        item.data.info.count = stringToCount(parts[1]);
    } else {
        item.data.info.iface = InterfaceType::Unknown;
        item.data.info.count = FiberCountType::Unknown;
    }

    item.data.info.spec     = stringToSpec(query.value("spec").toString());
    item.data.info.polarity = stringToPolarity(query.value("polarity").toString());
    item.data.info.mode     = stringToMode(query.value("mode").toString());
    item.data.qrTemplate    = query.value("qrTemplate").toString();
    item.data.labelTemplate = query.value("labelTemplate").toString();
    item.data.lambda.first  = query.value("lambda1").toInt();
    item.data.lambda.second = query.value("lambda2").toInt();
}
