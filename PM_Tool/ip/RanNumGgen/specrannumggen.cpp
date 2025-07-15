#include "specrannumggen.h"
#include <QRandomGenerator>
#include <QDebug>
specRanNumGgen::specRanNumGgen() {

}

double specRanNumGgen::generate_voltage_double() {
    return QRandomGenerator::global()->bounded(200u, 245.0);
}

double specRanNumGgen::generate_phase_current(double specification) {//电流随机生成函数
    return QRandomGenerator::global()->bounded(0, specification)/2.0;
}

double specRanNumGgen::get_power_factor_precise() { //功率因素随机
    return QRandomGenerator::global()->bounded(2, 10)/10.0;
}

double specRanNumGgen::get_thd()
{
    return QRandomGenerator::global()->bounded(1, 100)/1.0;
}

int specRanNumGgen::get_temperature() { // 实现温度
    return QRandomGenerator::global()->bounded(20u, 45u); // 明确无符号参数
}

double specRanNumGgen::getCurThd()
{
    return QRandomGenerator::global()->bounded(1,100) / 1.0;
}

int specRanNumGgen::get_humidity() {    //

    return QRandomGenerator::global()->bounded(5u, 96u);
}
double specRanNumGgen::getrandom(int x)//基数为100， 随机范围为 0.2 ~ 1
{
    return QRandomGenerator::global()->bounded(x/5.0,x)/100.0;
}
double specRanNumGgen::getrandominc(int x)
{
    double randomValue = QRandomGenerator::global()->generateDouble() * 2 * x*1.0 - x;
    return randomValue;
}
