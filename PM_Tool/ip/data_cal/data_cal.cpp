#include "data_cal.h"
#include <math.h>

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
double data_cal::reactive_powerCal(double S,double P)
{
    return sqrt(S*S - P*P)/1000.0;
}
