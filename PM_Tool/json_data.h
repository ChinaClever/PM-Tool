#ifndef JSON_DATA_H
#define JSON_DATA_H

#include <QObject>
#include <QJsonObject>
#include <QString>

class Json_Data : public QObject
{
    Q_OBJECT
public:
    explicit Json_Data(QObject *parent = nullptr);

    void setBasicInfo(int addr, int status, int version,    //赋值json的基本变量
                      const QString &dev_ip,
                      const QString &dev_key,
                      const QString &datetime,
                      const QString &pdu_alarm,
                      const QJsonObject &env_item_list,
                      const QJsonObject &line_item_list,
                      const QJsonObject &pdu_total_data,
                      const QJsonObject &pduData,
                      const QString &uuid
                      );

    // void setEnvItemList(const QJsonObject &env);
    // void setLineItemList(const QJsonObject &line);
    // void setPduTotalData(const QJsonObject &total);


    QJsonObject generateJson();                         //赋值json包
    QJsonObject getJsonObject() const;                  //返回json包

    QJsonObject generateLineItemList(int phaseCount,            //单三项电压赋值
                                    const QList<int> &curAlarmStatus,
                                    const QList<int> &curAlarmMax,
                                    const QList<double> &curValues,
                                    const QList<double> &eleActive,
                                    const QList<double> &powerFactor,
                                    const QList<double> &powValue,
                                    const QList<double> &powApparent,
                                    const QList<int> &powReactive,
                                    const QList<int> &volAlarmStatus,
                                    const QList<double> &volValues);

    QJsonObject generateEnvItemList(const QList<int> &insert,   //传感器数据赋值
                                    const QList<double> &humValue,
                                    const QList<double> &temValue,
                                    const QList<int> &humAlarmStatus,
                                    const QList<int> &temAlarmStatus);

    QJsonObject generatePduTotalData(double curRms,             //综合数据
                                     double volRms,
                                     double eleActive,
                                     double powerFactor,
                                     double powActive,
                                     double powApparent,
                                     double powReactive,
                                     double volUnbalance,
                                     double curUnbalance);

    QJsonObject generatePduData(const QJsonObject &envItem,     //pdu整合
                                const QJsonObject &lineItem,
                                const QJsonObject &totalData);

    QString generateVoltageAlarmDescription(const QVector<double>& vol_value,       //alarm输出内容，未实现
                                            const QVector<int>& vol_alarm_status);

    void show_addr();           //调试输出json是否正确

private:

    // 所有成员变量
    QJsonObject jsonObject;
    int m_addr;
    int m_status;
    int m_version;
    QString m_dev_ip;
    QString m_dev_key;
    QString m_datetime;
    QString m_pdu_alarm;
    QString m_uuid;

    QJsonObject m_env_item_list;
    QJsonObject m_line_item_list;
    QJsonObject m_pdu_total_data;
    QJsonObject m_pduData;
    QJsonObject m_json;
};

#endif // JSON_DATA_H
