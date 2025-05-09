#ifndef DATA_CAL_H
#define DATA_CAL_H

class data_cal
{
public:
    data_cal();

    static double apparent_powerCal(double V,double I);
    static double active_powerCal(double V,double I,double pf);
    static double reactive_powerCal(double app_pow,double reac_pow);
};

#endif // DATA_CAL_H
