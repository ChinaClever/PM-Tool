#ifndef MP_TOJSON_H
#define MP_TOJSON_H
#include "mp_globals.h"

#include <QObject>
#include <QJsonObject>
#include <QQueue>
#include <QJsonDocument>
#include <QJsonArray>
class mp_toJson:public QObject
{
    Q_OBJECT
public slots:
     void toJsonSlot(PowerSystemData);

public:
    explicit mp_toJson(QObject *parent = nullptr);
    mp_toJson();

    void setEnvItemList(PowerSystemData);
    void setLineItemList(PowerSystemData);
    void setLoopItemList(PowerSystemData);
    void setOutBitItemList(PowerSystemData);
    void setPduTotalData(PowerSystemData);
    void generatePduData(PowerSystemData);
    void setGlobalData(PowerSystemData);

    void setAlarm(const QString &alarm);


    void setDevkey();

    void setSerIP(const QString &serip);
    void setPort(const QString &port);
    QJsonObject generateJson();   //赋值Json包
    QJsonObject getJsonObject() const;

private:
    QString m_ser_ip;
    QString m_port;

    // 所有成员变量
    int m_addr;
    int m_status;
    int m_version = 1;
    int m_devhz = 55;

    QString m_dev_ip;
    QString m_dev_key;
    QString m_datetime;
    QString m_pdu_alarm;
    //QString m_uuid;

    QJsonObject m_env_item_list;
    QJsonObject m_line_item_list;
    QJsonObject m_loop_item_list;
    QJsonObject m_output_item_list;
    QJsonObject m_pdu_total_data;
    QJsonObject m_pduData;

    QJsonObject m_json;

};

#endif // MP_TOJSON_H
