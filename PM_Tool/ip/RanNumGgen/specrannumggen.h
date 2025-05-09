#ifndef SPECRANNUMGGEN_H
#define SPECRANNUMGGEN_H

class specRanNumGgen
{
public:
    specRanNumGgen();
    static double generate_voltage_double();
    static double generate_phase_current(double specification);
    static double get_power_factor_precise();

    static int get_temperature();

    static int get_humidity();
};

#endif // SPECRANNUMGGEN_H
