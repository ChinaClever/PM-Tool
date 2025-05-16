#include "ip_datapacket.h"
#include <math.h>
#include <numeric>
#include <QButtonGroup>
#include "data_cal/data_cal.h"
#include "specrannumggen.h"
template <int SIZE>
void IP_sDataPacket<SIZE>::totalDataCal() {

    this->total_item.ele_active = this->line_item.ele_active[0];
    this->total_item.pow_active = this->line_item.pow_value[0];
    this->total_item.pow_apparent = this->line_item.pow_apparent[0];
    this->total_item.pow_reactive = this->line_item.pow_reactive[0];

    for(int i=1;i<SIZE;i++){
        this->total_item.ele_active+=this->line_item.ele_active[i];

        this->total_item.pow_active+=this->line_item.pow_value[i];
        this->total_item.pow_apparent+=this->line_item.pow_apparent[i];
        this->total_item.pow_reactive+=this->line_item.pow_reactive[i];
    }

    this->total_item.power_factor = this->total_item.pow_apparent == 0 ? 0 :(this->total_item.pow_active/this->total_item.pow_apparent)*1.0;

    if(SIZE == 3){
    // 电压不平衡度：= Max({abs(voltages[0] – 平均电压), abs(voltages[1] -平均电压), abs(voltages[2] -平均电压)})

    // 电流不平衡度：Max({abs(current [0] – 平均电流), abs(current [1] -平均电流), abs(current [2] -平均电流)}
    double x = std::accumulate(
                   this->line_item.vol_value.begin(),
                   this->line_item.vol_value.end(),
                   0.0 // 初始值，必须是浮点型！
                   ) / SIZE;
        if(x == 0){
            this->total_item.vol_unbalance = 0;
        }
        else {
            this->total_item.vol_unbalance = ((*std::max_element(this->line_item.vol_value.begin(),this->line_item.vol_value.end()))
                                              -(*std::min_element(this->line_item.vol_value.begin(),this->line_item.vol_value.end())))
                                             /x*100;
            this->total_item.vol_unbalance = std::min(100.0,this->total_item.vol_unbalance);
        }
        x = std::accumulate(
                this->line_item.cur_value.begin(),
                this->line_item.cur_value.end(),
                0.0 // 初始值，必须是浮点型！
                ) / SIZE;
        if(x == 0){
            this->total_item.cur_unbalance = 0;
        }
        else{
            this->total_item.cur_unbalance = ((*std::max_element(this->line_item.cur_value.begin(),this->line_item.cur_value.end()))
                                              -(*std::min_element(this->line_item.cur_value.begin(),this->line_item.cur_value.end())))
                                             /x*100;
            this->total_item.cur_unbalance = std::min(100.0,this->total_item.cur_unbalance);
        }
    }
}
template <int SIZE>
void IP_sDataPacket<SIZE>::intiline(const int curCap)
{
    for(int i=0;i<SIZE;i++){
        { //line——Item
            this->line_item.cur_alarm_max[i] = curCap;
            this->line_item.cur_value[i] = specRanNumGgen::generate_phase_current(curCap);
            this->line_item.vol_value[i] = specRanNumGgen::generate_voltage_double();
            this->line_item.cur_alarm_status[i] = 0;

            this->line_item.vol_alarm_status[i] = 0;
            this->line_item.power_factor[i] = specRanNumGgen::get_power_factor_precise(); //功率因素
            this->line_item.pow_apparent[i] = data_cal::apparent_powerCal(this->line_item.vol_value[i],this->line_item.cur_value[i]);
            this->line_item.pow_value[i] = data_cal::active_powerCal(this->line_item.vol_value[i],this->line_item.cur_value[i],this->line_item.power_factor[i]);
            this->line_item.pow_reactive[i] = data_cal::reactive_powerCal(this->line_item.pow_apparent[i],this->line_item.pow_value[i]);
        }
    }
}
// 显式实例化模板
template struct IP_sDataPacket<3>;  // 假设实际使用 SIZE=3
template struct IP_sDataPacket<1>;  // 假设实际使用 SIZE=1
