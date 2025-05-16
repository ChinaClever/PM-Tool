#ifndef IP_SENDJSON_H
#define IP_SENDJSON_H
#include <QJsonObject>
#include <QString>
#include <QObject>
#include <QElapsedTimer>
class ip_sendJson : public QObject
{
    Q_OBJECT
public:

    explicit ip_sendJson(QObject *parent = nullptr);

    void setEnvItemList(const QJsonObject &env);
    void setLineItemList(const QJsonObject &line);
    void setPduTotalData(const QJsonObject &total);
    void generatePduData(const QJsonObject &envItem);
    void setStatus(const int &x);
    void setAlarm(const QString &alarm);

    void setDatetime(const QString &datetime);
    void setAddr(const int &addr);
    void setDevIp(const QString &devip);
    void setDevkey();

    void setSerIP(const QString &serip);
    void setPort(const QString &port);
    QJsonObject generateJson();   //赋值Json包
    QJsonObject getJsonObject() const;                  //返回json包

public slots:
    void sendTogoal(const QJsonObject &json,const QString &serip,const QString &port);

private:

    QElapsedTimer m_lastValidSend; // 记录上一次有效发送的时间戳
    static const int INTERVAL_THRESHOLD = 500;

    QString m_ser_ip;
    QString m_port;

    // 所有成员变量
    int m_addr;
    int m_status;
    int m_version = 1;
    QString m_dev_ip;
    QString m_dev_key;
    QString m_datetime;
    QString m_pdu_alarm;
    //QString m_uuid;

    QJsonObject m_env_item_list;
    QJsonObject m_line_item_list;
    QJsonObject m_pdu_total_data;
    QJsonObject m_pduData;

    QJsonObject m_json;
};

#endif // IP_SENDJSON_H
