#include "templatesql.h"

TemplateSql::TemplateSql(QObject* parent)
    : FiberBaseSql(parent)
{
}

QString TemplateSql::tableName() const
{
    return "templates";
}

void TemplateSql::createTable()
{
    const char* ddl =
        "CREATE TABLE IF NOT EXISTS templates ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  PN TEXT NOT NULL UNIQUE,"
        "  description TEXT,"
        "  polarity TEXT,"
        "  iface TEXT,"
        "  qrTemplate TEXT,"
        "  FanoutPn TEXT,"
        "  FanCount INTEGER,"
        "  labelTemplate INTEGER,"
        "  lambda1 INTEGER,"
        "  lambda2 INTEGER,"
        "  limit_value REAL DEFAULT 0.0,"
        "  mode TEXT,"
        "  spec TEXT,"
        "  fibercount INTEGER"
        ");";
    QSqlQuery q(database());
    q.exec(ddl);
}

bool TemplateSql::exists(const QString& pn)
{
    QSqlQuery q(database());
    q.prepare("SELECT 1 FROM templates WHERE PN = ? LIMIT 1");
    q.bindValue(0, pn);
    if (!q.exec()) { handleError(q.lastError()); return false; }
    return q.next();
}

bool TemplateSql::insertOne(const sTemInfo& info)
{
    QSqlQuery q(database());
    // q.prepare(
    //     "INSERT INTO templates ("
    //     " PN, FanoutPn, description, FanCount, limit_value,"
    //     " qrTemplate, labelTemplate,"     /* 新增字段 */
    //     " iface, count, spec, polarity, mode"
    //     ") VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?)"
    //     );

    // q.bindValue(0,  info.PN);
    // q.bindValue(1,  info.FanoutPn);
    // q.bindValue(2,  info.description);
    // q.bindValue(3,  info.FanCount);
    // q.bindValue(4,  info.limit);
    // q.bindValue(5,  info.qrTemplate);
    // q.bindValue(6,  info.labelTemplate);
    // q.bindValue(7,  ifaceToString(info.info.iface));
    // q.bindValue(8,  countToString(info.info.count));
    // q.bindValue(9,  specToString(info.info.spec));
    // q.bindValue(10, polarityToString(info.info.polarity));
    // q.bindValue(11, modeToString(info.info.mode));

    if (!q.exec()) { handleError(q.lastError()); return false; }
    emit itemChanged(q.lastInsertId().toInt(), Operation::Insert);
    return true;
}

bool TemplateSql::upsert(const sTemInfo& info)
{
    if (exists(info.PN)) {
        QSqlQuery del(database());
        del.prepare("DELETE FROM templates WHERE PN = ?");
        del.bindValue(0, info.PN);
        if (!del.exec()) { handleError(del.lastError()); return false; }
    }
    return insertOne(info);
}

bool TemplateSql::findByPN(const QString& pn, sTemInfo& out)
{
    QSqlQuery q(database());
    q.prepare("SELECT * FROM templates WHERE PN = ?");
    q.bindValue(0, pn);
    if (!q.exec()) { handleError(q.lastError()); return false; }
    if (!q.next()) return false;
    out = rowToTemInfo(q);
    return true;
}

QVector<sTemInfo> TemplateSql::findAll()
{
    QVector<sTemInfo> res;
    QSqlQuery q(database());
    q.prepare("SELECT * FROM templates ORDER BY id DESC");
    if (!q.exec()) { handleError(q.lastError()); return res; }
    while (q.next()) res.append(rowToTemInfo(q));
    return res;
}

sTemInfo TemplateSql::rowToTemInfo(QSqlQuery& q) const
{
    sTemInfo t;
    t.PN          = q.value("PN").toString();
    t.FanoutPn    = q.value("FanoutPn").toString();
    t.description = q.value("description").toString();
    t.FanCount    = q.value("FanCount").toInt();
    t.limit       = q.value("limit_value").toDouble();
    t.qrTemplate  = q.value("qrTemplate").toString();      // 新增
    t.labelTemplate = q.value("labelTemplate").toInt();    // 新增

    const auto ifaceStr  = q.value("iface").toString();
    const auto countStr  = q.value("fiber_count").toString();
    const auto specStr   = q.value("spec").toString();
    const auto polaStr   = q.value("polarity").toString();
    const auto modeStr   = q.value("mode").toString();

    if      (ifaceStr == "MTP-LC") t.info.iface = InterfaceType::MTP_LC;
    else if (ifaceStr == "MTP-SN") t.info.iface = InterfaceType::MTP_SN;
    else if (ifaceStr == "MTP-MDC")t.info.iface = InterfaceType::MTP_MDC;
    else                           t.info.iface = InterfaceType::Unknown;

    if      (countStr == "8F")  t.info.count = FiberCountType::F8;
    else if (countStr == "12F") t.info.count = FiberCountType::F12;
    else if (countStr == "16F") t.info.count = FiberCountType::F16;
    else                        t.info.count = FiberCountType::Unknown;

    if      (specStr == "OS2")       t.info.spec = FiberSpec::OS2;
    else if (specStr == "OM4 Aqua")  t.info.spec = FiberSpec::OM4_Aqua;
    else if (specStr == "OM4 HV")    t.info.spec = FiberSpec::OM4_HV;
    else if (specStr == "OM5")       t.info.spec = FiberSpec::OM5;
    else                             t.info.spec = FiberSpec::Unknown;

    if      (polaStr == "UNIVERSAL") t.info.polarity = Polarity::UNIVERSAL;
    else if (polaStr == "AC")        t.info.polarity = Polarity::AC;
    else                             t.info.polarity = Polarity::Unknown;

    if      (modeStr == "SM") t.info.mode = FiberMode::SM;
    else if (modeStr == "MM") t.info.mode = FiberMode::MM;
    else                      t.info.mode = FiberMode::Unknown;

    return t;
}
