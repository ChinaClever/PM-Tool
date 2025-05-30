#ifndef SPECRANNUMGGEN_H
#define SPECRANNUMGGEN_H

class specRanNumGgen
{
public:
    specRanNumGgen();
    static double generate_voltage_double();
    static double generate_phase_current(double specification);
    static double get_power_factor_precise();

    static int get_temperature();  //随机温度（15~66）
    static double getrandom(int x); //随机
    static int get_humidity();      //随机湿度 （5~96）
    static double getrandominc(int x);
};

#endif // SPECRANNUMGGEN_H
