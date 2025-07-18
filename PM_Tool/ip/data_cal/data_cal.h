#ifndef DATA_CAL_H
#define DATA_CAL_H
#include <QString>

class data_cal
{
public:
    data_cal();

    static double apparent_powerCal(double V,double I); //视在功率计算
    static double active_powerCal(double V,double I,double pf);  //有功功率
    static double reactive_powerCal(double app_pow,double pow);//无功功率
    static qint64 calculateTimeDiffInSeconds(const QString& timeStr1, const QString& timeStr2); //返回两个时间差值
    static QString generateNextCascadeIP(QString &devip, int &addr,int addnum);  //返回下一个key，devip，addr递增

    static double calculate_dewpoint1(double t, double h) ;
    static double calculateUnbalance(double iA, double iB, double iC);
    static QString getNextIp(const QString &ip);

};

#endif // DATA_CAL_H
