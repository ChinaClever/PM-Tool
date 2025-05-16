#include "ip_sendjson.h"
#include <QUdpSocket>
#include <QJsonDocument>
ip_sendJson::ip_sendJson(QObject *parent)
    : QObject(parent)
{

}
QJsonObject ip_sendJson::generateJson() {
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

    m_json = root;
   // qDebug()<<m_json;
    return root;
}
QJsonObject ip_sendJson::getJsonObject() const
{
    return m_json;
}
void ip_sendJson::setEnvItemList(const QJsonObject &line) {
    m_env_item_list = line;
}
void ip_sendJson::setLineItemList(const QJsonObject &line){
   m_line_item_list = line;
}
void ip_sendJson::setPduTotalData(const QJsonObject &line)
{
    m_pdu_total_data = line;
}
void ip_sendJson::generatePduData(const QJsonObject &line)
{
    m_pduData = line;
}
void ip_sendJson::setStatus(const int &x)
{
    m_status = x;
}
void ip_sendJson::setAlarm(const QString &alarm)
{
    m_pdu_alarm = alarm;
}
void ip_sendJson::setDatetime(const QString &datetime)
{
    m_datetime = datetime;
}
void ip_sendJson::setAddr(const int &addr)
{
    m_addr = addr;
}
void ip_sendJson::setDevIp(const QString &devip)
{
    m_dev_ip = devip;
}
void ip_sendJson::setDevkey()
{
    QString dev_key = m_dev_ip + "-" + QString("%1").arg(m_addr, 1, 10, QChar('0'));
    m_dev_key = dev_key;
}
void ip_sendJson::setSerIP(const QString &serip)
{
    m_ser_ip = serip;
}
void ip_sendJson::setPort(const QString &port)
{
    m_port = port;
}

void ip_sendJson::sendTogoal(const QJsonObject &json,const QString &serip,const QString &port) {
    // 检查是否已记录有效时间戳
    if (m_lastValidSend.isValid()) {
        // 计算距离上一次有效发送的时间差
        qint64 elapsed = m_lastValidSend.elapsed();
        if (elapsed < INTERVAL_THRESHOLD) {
            // 时间间隔小于 500ms，直接丢弃数据
            return;
        }
    }
    QUdpSocket *udpsocket = new QUdpSocket(this);
    QByteArray jsonData = QJsonDocument(json).toJson(QJsonDocument::Compact);
    QString ipAddress = serip;
    QString portStr = port;
    quint16 Port = portStr.toUShort();
    qDebug() << "发送目标 IP:" << ipAddress << "Port:" << Port; // 添加此行打印目标地址
    udpsocket->writeDatagram(jsonData, QHostAddress(ipAddress), Port);
    udpsocket->deleteLater();


    // 执行打印操作
    qDebug() << "=========================\n" << json;
    // 更新有效时间戳
    m_lastValidSend.start(); // 首次调用或间隔达标后记录时间戳
}
