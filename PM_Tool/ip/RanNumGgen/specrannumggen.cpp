#include "specrannumggen.h"
#include <QRandomGenerator>

specRanNumGgen::specRanNumGgen() {

}

double specRanNumGgen::generate_voltage_double() {
    return QRandomGenerator::global()->bounded(200u, 245.0);
}

double specRanNumGgen::generate_phase_current(double specification) {//电流随机生成函数
    return QRandomGenerator::global()->bounded(0, specification)/2.0;
}

double specRanNumGgen::get_power_factor_precise() {
    return QRandomGenerator::global()->bounded(2, 10)/10.0;
}

int specRanNumGgen::get_temperature() { // 实现温度
    return QRandomGenerator::global()->bounded(15u, 66u); // 明确无符号参数
}

int specRanNumGgen::get_humidity() {    //

    return QRandomGenerator::global()->bounded(5u, 96u);
}
