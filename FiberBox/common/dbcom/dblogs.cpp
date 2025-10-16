#include "dblogs.h"
#include <QDebug>

DbLogs::DbLogs()
{
    createTable();
    tableTile = tr("光纤日志");
    headList << tr("光纤盒ID") << tr("成品编号")
             << tr("FanoutPn") << tr("描述") << tr("扇出线ID") << tr("波类型")
             << tr("最大限值IL") << tr("二维码内容")
             << tr("扇出线1序列号") << tr("扇出线1二维码内容")
             << tr("扇出线2序列号") << tr("扇出线2二维码内容")
             << tr("扇出线3序列号") << tr("扇出线3二维码内容")
             << tr("扇出线4序列号") << tr("扇出线4二维码内容");
}

DbLogs* DbLogs::build()
{
    static DbLogs* instance = nullptr;
    if(!instance) instance = new DbLogs();
    return instance;
}

void DbLogs::createTable()
{
    QString cmd = QString(
                      "CREATE TABLE IF NOT EXISTS %1 ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                      "date TEXT,"
                      "time TEXT,"
                      "boxId TEXT,"
                      "PN TEXT,"
                      "fanoutPn TEXT,"
                      "description TEXT,"
                      "fanoutId TEXT,"
                      "waveType TEXT,"
                      "limitIL REAL DEFAULT 0.0,"
                      "qrContent TEXT,"
                      "seq1 TEXT, qr1 TEXT,"
                      "seq2 TEXT, qr2 TEXT,"
                      "seq3 TEXT, qr3 TEXT,"
                      "seq4 TEXT, qr4 TEXT"
                      ");"
                      ).arg(tableName());

    QSqlQuery query(mDb);
    if(!query.exec(cmd)) {
        throwError(query.lastError());
    }
}

bool DbLogs::insertItem(const sFiberLogItem &item)
{
    QString cmd =
        "INSERT INTO %1 (date, time, boxId, PN, fanoutPn, description, fanoutId, waveType, limitIL, qrContent,"
        "seq1, qr1, seq2, qr2, seq3, qr3, seq4, qr4) "
        "VALUES (:date, :time, :boxId, :PN, :fanoutPn, :description, :fanoutId, :waveType, :limitIL, :qrContent,"
        ":seq1, :qr1, :seq2, :qr2, :seq3, :qr3, :seq4, :qr4)";

    QSqlQuery query(mDb);
    query.prepare(cmd.arg(tableName()));

    query.bindValue(":date", item.date);
    query.bindValue(":time", item.time);
    query.bindValue(":boxId", item.boxId);
    query.bindValue(":PN", item.PN);
    query.bindValue(":fanoutPn", item.fanoutPn);
    query.bindValue(":description", item.description);
    query.bindValue(":fanoutId", item.fanoutId);
    query.bindValue(":waveType", item.waveType);
    query.bindValue(":limitIL", item.limitIL);
    query.bindValue(":qrContent", item.qrContent);

    query.bindValue(":seq1", item.seq1);
    query.bindValue(":qr1", item.qr1);
    query.bindValue(":seq2", item.seq2);
    query.bindValue(":qr2", item.qr2);
    query.bindValue(":seq3", item.seq3);
    query.bindValue(":qr3", item.qr3);
    query.bindValue(":seq4", item.seq4);
    query.bindValue(":qr4", item.qr4);

    bool ret = query.exec();
    if(!ret) throwError(query.lastError());

    return ret;
}

void DbLogs::selectItem(QSqlQuery &query, sFiberLogItem &item)
{
    item.id = query.value("id").toInt();
    item.date = query.value("date").toString();
    item.time = query.value("time").toString();
    item.boxId = query.value("boxId").toString();
    item.PN = query.value("PN").toString();
    item.fanoutPn = query.value("fanoutPn").toString();
    item.description = query.value("description").toString();
    item.fanoutId = query.value("fanoutId").toString();
    item.waveType = query.value("waveType").toString();
    item.limitIL = query.value("limitIL").toDouble();
    item.qrContent = query.value("qrContent").toString();

    item.seq1 = query.value("seq1").toString();
    item.qr1 = query.value("qr1").toString();
    item.seq2 = query.value("seq2").toString();
    item.qr2 = query.value("qr2").toString();
    item.seq3 = query.value("seq3").toString();
    item.qr3 = query.value("qr3").toString();
    item.seq4 = query.value("seq4").toString();
    item.qr4 = query.value("qr4").toString();
}
