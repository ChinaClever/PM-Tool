#ifndef BUS_TOJSON_H
#define BUS_TOJSON_H
#include "bus/data_struct/BusData.h"

#include <QObject>
#include <QJsonObject>
#include <QQueue>
#include <QJsonDocument>
#include <QJsonArray>
class bus_toJson:public QObject
{
    Q_OBJECT
public:
    explicit bus_toJson(QObject *parent = nullptr);
    bus_toJson();
    QJsonObject toJson(Busbar& data,bool );


    void setBoxCfg(Busbar& );//插接箱配置
    void setLoop(Busbar& ); //回路
    void setBoxLine(Busbar&); //插接箱相数据
    void setOutLet(Busbar&);    //插接箱输出位数据
    void setBoxTotal(Busbar& );//插接箱总数据

    void setBusCfg(Busbar&);//母线配置
    void setBusLine(Busbar&);//母线相数据
    void setBusTotal(Busbar& );
    void setBusData();

    void setJson(bool flag);

    void setBoxData();

    void setGlobalData(Busbar& systemData);
    void setTem(Busbar& systemData, bool);



private:

    bool st = 0; //alarm status

    QString alarm = "";
    QString m_ser_ip;
    QString m_port;

    // 所有成员变量
    int m_addr;
    int m_status;
    QString m_version = "V3.0.3";

    QString m_dev_ip;
    int m_bar_id;
    QString m_datetime;
    QString m_dev_alarm;

    QString m_bus_name;
    QString m_box_name;
    QString m_bus_key;
    QString m_box_key;

    QJsonObject m_bus_data;
    QJsonObject m_bus_cfg;
    QJsonObject m_line_item_list;
    QJsonObject m_bus_total_data;

    QJsonObject m_box_data;
    QJsonObject m_box_cfg;
    QJsonObject m_loop_item_list;
    QJsonObject m_box_line_item;
    QJsonObject m_outlet_item_list;
    QJsonObject m_box_total_data;
    QJsonObject m_env_item_list;




    QJsonObject m_json;
};

#endif // BUS_TOJSON_H
