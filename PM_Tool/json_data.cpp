#include "json_data.h"
#include <QArrayData>
#include <QJsonArray>

Json_Data::Json_Data(QObject *parent)
    : QObject(parent)
{

}

void Json_Data::show_addr()
{
    qDebug()<<m_addr;
}
// QJsonObject m_env_item_list;
// QJsonObject m_line_item_list;
// QJsonObject m_pdu_total_data;
// QJsonObject m_pduData;
void Json_Data::setBasicInfo(int addr, int status, int version,
                             const QString &dev_ip,
                             const QString &dev_key,
                             const QString &datetime,
                             const QString &pdu_alarm,
                             const QJsonObject &env_item_list,
                             const QJsonObject &line_item_list,
                             const QJsonObject &pdu_total_data,
                             const QJsonObject &pduData,
                             const QString &uuid
                             )
{
    m_addr = addr;
    m_status = status;
    m_version = version;
    m_dev_ip = dev_ip;
    m_dev_key = dev_key;
    m_datetime = datetime;
    m_pdu_alarm = pdu_alarm;
    m_uuid = uuid;
    m_env_item_list = env_item_list;
    m_line_item_list = line_item_list;
    m_pdu_total_data = pdu_total_data;
    m_pduData = pduData;
}

// void Json_Data::setEnvItemList(const QJsonObject &env) {
//     m_env_item_list = env;
// }

// void Json_Data::setLineItemList(const QJsonObject &line) {
//     m_line_item_list = line;
// }

// void Json_Data::setPduTotalData(const QJsonObject &total) {
//     m_pdu_total_data = total;
// }

QJsonObject Json_Data::generateJson() {
    QJsonObject root;
    root["addr"] = m_addr;
    root["status"] = m_status;
    root["version"] = m_version;
    root["dev_ip"] = m_dev_ip;
    root["dev_key"] = m_dev_key;
    root["datetime"] = m_datetime;

    QJsonObject pdu_data;
    pdu_data["env_item_list"] = m_env_item_list;
    pdu_data["line_item_list"] = m_line_item_list;
    pdu_data["pdu_total_data"] = m_pdu_total_data;
    root["pdu_data"] = pdu_data;

    root["pdu_alarm"] = m_pdu_alarm;
    if (!m_uuid.isEmpty()) {
        root["uuid"] = m_uuid;
    }

    jsonObject = root;
    return root;
}

QJsonObject Json_Data::getJsonObject() const {
    return jsonObject;
}

QJsonObject Json_Data::generateLineItemList(
    int phaseCount,
    const QList<int> &curAlarmStatus,
    const QList<int> &curAlarmMax,
    const QList<double> &curValues,
    const QList<double> &eleActive,
    const QList<double> &powerFactor,
    const QList<double> &powValue,
    const QList<double> &powApparent,
    const QList<int> &powReactive,
    const QList<int> &volAlarmStatus,
    const QList<double> &volValues
    ) {
    QJsonObject lineItem;

    QJsonArray curAlarmStatusArray, curAlarmMaxArray, curValueArray;
    QJsonArray eleActiveArray, powerFactorArray, powValueArray, powApparentArray;
    QJsonArray volAlarmStatusArray, volValueArray,powReactiveArray;

    for (int i = 0; i < phaseCount; ++i) {
        curAlarmStatusArray.append(curAlarmStatus[i]);
        curAlarmMaxArray.append(curAlarmMax[i]);
        curValueArray.append(curValues[i]);
        eleActiveArray.append(eleActive[i]);
        powerFactorArray.append(powerFactor[i]);
        powValueArray.append(powValue[i]);
        powApparentArray.append(powApparent[i]);
        powReactiveArray.append(powReactive[i]);
        volAlarmStatusArray.append(volAlarmStatus[i]);
        volValueArray.append(volValues[i]);
    }

    lineItem["cur_alarm_status"] = curAlarmStatusArray;
    lineItem["cur_alarm_max"] = curAlarmMaxArray;
    lineItem["cur_value"] = curValueArray;
    lineItem["ele_active"] = eleActiveArray;
    lineItem["power_factor"] = powerFactorArray;
    lineItem["pow_value"] = powValueArray;
    lineItem["pow_apparent"] = powApparentArray;
    lineItem["pow_reactive"] = powReactiveArray;
    lineItem["vol_alarm_status"] = volAlarmStatusArray;
    lineItem["vol_value"] = volValueArray;

    return lineItem;
}


QJsonObject Json_Data::generateEnvItemList(const QList<int> &insert,
                                           const QList<double> &humValue,
                                           const QList<double> &temValue,
                                           const QList<int> &humAlarmStatus,
                                           const QList<int> &temAlarmStatus)
{
    QJsonObject envItem;

    if (insert.size() != 2 || humValue.size() != 2 || temValue.size() != 2 ||
        humAlarmStatus.size() != 2 || temAlarmStatus.size() != 2) {
        qWarning("generateEnvItemList 参数数量不正确，必须都是2个");
        return QJsonObject();  // 参数不合法
    }

    // 判断两个传感器是否都未插入
    if (insert[0] == 0 && insert[1] == 0) {
        return QJsonObject(); // 不写 env_item_list
    }

    // 只要有一个插入，就写出完整的结构
    QJsonArray insertArr, humArr, temArr, humAlarmArr, temAlarmArr;
    for (int i = 0; i < 2; ++i) {
        insertArr.append(insert[i]);

        // 如果未插入，值置0（也可以用null，根据协议定义）
        humArr.append(insert[i] ? humValue[i] : 0);
        temArr.append(insert[i] ? temValue[i] : 0);
        humAlarmArr.append(insert[i] ? humAlarmStatus[i] : 0);
        temAlarmArr.append(insert[i] ? temAlarmStatus[i] : 0);
    }

    envItem["insert"] = insertArr;
    envItem["hum_value"] = humArr;
    envItem["tem_value"] = temArr;
    envItem["hum_alarm_status"] = humAlarmArr;
    envItem["tem_alarm_status"] = temAlarmArr;

    return envItem;
}

QJsonObject Json_Data::generatePduTotalData(double curRms,
                                            double volRms,
                                            double eleActive,
                                            double powerFactor,
                                            double powActive,
                                            double powApparent,
                                            double powReactive,
                                            double volUnbalance,
                                            double curUnbalance)
{
    QJsonObject totalData;

    totalData["cur_rms"] = curRms;
    totalData["vol_rms"] = volRms;
    totalData["ele_active"] = eleActive;
    totalData["power_factor"] = powerFactor;
    totalData["pow_active"] = powActive;
    totalData["pow_apparent"] = powApparent;
    totalData["pow_reactive"] = 0; // 预留字段不需要写
    totalData["vol_unbalance"] = volUnbalance;
    totalData["cur_unbalance"] = curUnbalance;

    return totalData;
}

QJsonObject Json_Data::generatePduData(const QJsonObject &envItem,
                                       const QJsonObject &lineItem,
                                       const QJsonObject &totalData)
{
    QJsonObject pduData;

    // 如果 envItem 不是空的才添加
    if (!envItem.isEmpty()) {
        pduData["env_item_list"] = envItem;
    }

    pduData["line_item_list"] = lineItem;
    pduData["pdu_total_data"] = totalData;
    return pduData;
}

QString Json_Data::generateVoltageAlarmDescription(const QVector<double>& vol_value, const QVector<int>& vol_alarm_status)
{
    QString alarmText;
    const double alarmMin = 80.0;
    const double alarmMax = 286.0;
    const double warningMin = 80.0;
    const double warningMax = 286.0;

    for (int i = 0; i < vol_value.size(); ++i) {
        if (vol_alarm_status[i] != 0) {
            QString desc = QString("本机第%1相电压, ").arg(i + 1);

            if (vol_value[i] < alarmMin) {
                desc += "过低告警";
            } else if (vol_value[i] > alarmMax) {
                desc += "过高告警";
            } else {
                desc += "异常告警"; // 范围内但状态标志不为0，也可能是预警等
            }

            desc += QString("; 当前值:%1V\n").arg(vol_value[i]);
            desc += QString("告警最小值:%1V 预警最小值:%2V 预警最大值:%3V 告警最大值:%4V;\n\n")
                        .arg(alarmMin)
                        .arg(warningMin)
                        .arg(warningMax)
                        .arg(alarmMax);

            alarmText += desc;
        }
    }

    return alarmText;
}
