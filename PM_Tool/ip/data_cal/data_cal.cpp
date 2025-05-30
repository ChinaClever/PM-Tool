#include "data_cal.h"
#include <math.h>
#include <QDateTime>
#include "ip_bulksend/ip_bulksend.h"

data_cal::data_cal() {}

/**
 * @brief XX规律计算
 * @param 电压
 * @param 电流
 * @param 功率因素
 */

double data_cal::apparent_powerCal(double V,double I)
{
    return V*I/1000.0;
}

double data_cal::active_powerCal(double V,double I,double pf)
{
    return V*I*pf/1000.0;
}

double data_cal::reactive_powerCal(double S,double P) //S:视在功率
{
    return sqrt(fabs(S*S - P*P));
}

qint64 data_cal::calculateTimeDiffInSeconds(const QString& timeStr1, const QString& timeStr2) {
    // 定义时间格式（根据你的实际格式调整）
    const QString format = "yyyy-MM-dd HH:mm:ss";

    // 转换为QDateTime
    QDateTime dt1 = QDateTime::fromString(timeStr1, format);
    QDateTime dt2 = QDateTime::fromString(timeStr2, format);


    // 计算时间差（大减小）
    return qAbs(dt1.secsTo(dt2));
}

QString data_cal::generateNextCascadeIP(QString &devip, int &addr,int addnum)
{
    QStringList ipSegments = devip.split('.');
    if (ipSegments.size() != 4) return QString(); // 确保IPv4格式

    // 从最后一段向前遍历处理进位
    bool carry = false;

    // 1. 处理addr递增
    if (++addr >= addnum) {
        addr = 0;
        carry = true; // 触发IP段进位
    }

    // 2. 仅在需要时处理IP段进位
    if (carry) {
        for (int i = 3; i >= 0; i--) {
            int segment = ipSegments[i].toInt();
            if (++segment > 255) {    // 段溢出
                segment = 0;          // 归零
                ipSegments[i] = QString::number(segment);
            } else {                 // 未溢出，停止进位
                ipSegments[i] = QString::number(segment);
                break;
            }
        }
        devip = ipSegments.join('.');    // 更新IP
    }
    QString dev_key = QString("%1-%2")
                          .arg(devip)
                          .arg(addr, 1, 10, QChar('0'));

    return dev_key;
}

double data_cal::calculate_dewpoint1(double t, double h) {
    double logex = (std::log10(h) - 2) / 0.4343 + (17.62 * t) / (243.12 + t);
    double dew = 243.12 * logex / (17.62 - logex);
    if (t == 0.0 && h == 0.0) {
        return 0.0; // 特殊处理0℃/0%的情况
    }
    // 四舍五入到两位小数
    return round(dew * 100) / 100;
}
