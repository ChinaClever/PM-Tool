#include "bus_tojson.h"

bus_toJson::bus_toJson(QObject *parent) : QObject(parent)
{

}

QJsonObject bus_toJson::toJson(Busbar& data,bool flag)
{

    setGlobalData(data);
    setTem(data,flag);
    if(flag){
        setBoxCfg(data);
        setLoop(data);
        setBoxLine(data);
        setOutLet(data);
        setBoxTotal(data);
        setBoxData();
    }
    else{
        //data.info.addr = 1;
        setBusCfg(data);
        setBusLine(data);
        setBusTotal(data);
        setBusData();
    }
    setJson(flag);

    return m_json;
}

void bus_toJson::setBusData()
{
    QJsonObject bus;
    bus["bus_cfg"] = m_bus_cfg;
    bus["line_item_list"] = m_line_item_list;
    bus["bus_total_data"] = m_bus_total_data;

    m_bus_data = bus;

}

void bus_toJson::setBusCfg(Busbar& data)
{
    auto &u = data.busData.busCfg;
    QJsonObject cfg;

    cfg["cur_specs"] = u.curSpecs;
    cfg["work_mode"] = u.workMode;
    cfg["beep"] = u.beep;
    cfg["ac_dc"] = u.acDc;
    cfg["box_num"] = u.boxNum;
    cfg["iof"] = u.iof;
    cfg["isd"] = u.isd;
    cfg["shunt_trip"] = u.shuntTrip;
    cfg["breaker_status"] = u.breakerStatus;
    cfg["lsp_status"] = u.lspStatus;
    cfg["bus_version"] = "2";
    cfg["item_type"] = u.itemType;
    cfg["baud_rate"] = u.baudRate;
    cfg["alarm_count"] = u.alarmCount;

    m_bus_cfg = cfg;
}

void bus_toJson::setBusTotal(Busbar& data)
{
    QJsonObject total;
    auto &u = data.busData.busTotalData;

    total["pow_value"] = qRound(u.powValue * 1000) / 1000.0;
    total["pow_min"] = u.powMin;
    total["pow_max"] = u.powMax;
    total["pow_status"] = u.powStatus;

    total["pow_apparent"] = qRound(u.powApparent * 1000) / 1000.0;
    total["pow_reactive"] = qRound(u.powReactive * 1000) / 1000.0;

    total["power_factor"] = qRound(u.powerFactor * 100) / 100.0;

    total["ele_active"] = qRound(u.eleActive*1)/1.0;//qRound(systemData.pduData.totalData.eleActive*1)/1.0;
    total["ele_apparent"] = qRound(u.eleApparent*1)/1.0;
    total["ele_reactive"] = qRound(u.eleReactive*1)/1.0;

    total["cur_residual_value"] = u.curResidualValue;
    total["cur_residual_alarm"] = u.curResidualAlarm;
    total["cur_residual_status"] = u.curResidualStatus;

    total["cur_zero_value"] = u.curZeroValue;
    total["cur_zero_alarm"] = u.curZeroAlarm;
    total["cur_zero_status"] = u.curZeroStatus;

    total["vol_unbalance"] = u.volUnbalance;
    total["cur_unbalance"] = qRound(u.curUnbalance * 100) / 100.0;

    total["hz_value"] = u.hzValue;
    total["hz_min"] = u.hzMin;
    total["hz_max"] = u.hzMax;
    total["hz_status"] = u.hzStatus;

    m_bus_total_data = total;
}

void bus_toJson::setBusLine(Busbar& data)
{
    QJsonArray vol_value,vol_min,vol_max,vol_status,vol_thd,cur_thd,cur_value,cur_min,cur_max
                ,cur_status,pow_value,pow_min,pow_max,pow_status,pow_apparent,pow_reactive,ele_active
                ,ele_reactive,power_factor,load_rate,vol_line_value,vol_line_min
                ,vol_line_max,vol_line_status;

    QJsonObject line;
    auto &u = data.busData.lineItemList;

    for (int i = 0; i < 3; i++) {
        vol_value.append(u.volValue[i]);
        vol_min.append(u.volMin[i]);
        vol_max.append(u.volMax[i]);
        if (u.volStatus[i]) {
            st = 1;
            alarm += QString("始端箱相电压过高，当前电压 %1 V，最大电压 %2 V\n")
                         .arg(u.volValue[i])
                         .arg(u.volMax[i]);
        }

        vol_status.append(u.volStatus[i]);
        vol_thd.append(u.volThd[i]);
        cur_thd.append(u.curThd[i]);
        cur_value.append(qRound(u.curValue[i] * 100) / 100.0);

        cur_min.append(u.curMin[i]);
        cur_max.append(u.curMax[i]);
        if (u.curStatus[i]) {
            st = 1;
            alarm += QString("始端箱相电流过高，当前电流 %1 A，最大电流 %2 A")
                         .arg(u.curValue[i])
                         .arg(u.curMax[i]);
        }

        cur_status.append(u.curStatus[i]);

        pow_value.append(qRound(u.powValue[i] * 1000) / 1000.0);

        pow_min.append(u.powMin[i]);
        pow_max.append(u.powMax[i]);
        if (u.powStatus[i]) {
            st = 1;
            alarm += QString("始端箱相功率过高，当前功率 %1 kW，最大功率 %2 kW\n")
                         .arg(u.powValue[i])
                         .arg(u.powMax[i]);
        }
        pow_status.append(u.powStatus[i]);
        pow_apparent.append(qRound(u.powApparent[i] * 1000) / 1000.0);  // 保留3位小数
        pow_reactive.append(qRound(u.powReactive[i] * 1000) / 1000.0);  // 保留3位小数


        ele_active.append(qRound(u.eleActive[i]*1)/1.0);
        ele_reactive.append(qRound(u.eleReactive[i]*1)/1.0);

        power_factor.append(qRound(u.powerFactor[i] * 100) / 100.0);  // 功率因数保留 3 位小数
        load_rate.append(qRound(u.loadRate[i] * 100) / 100.0);           // 负载率保留 2 位小数


        vol_line_value.append(qRound(u.volLineValue[i] * 100) / 100.0);

        vol_line_min.append(u.volLineMin[i]);
        vol_line_max.append(u.volLineMax[i]);
        vol_line_status.append(u.volLineStatus[i]);
    }

    line["vol_value"] = vol_value;
    line["vol_min"] = vol_min;
    line["vol_max"] = vol_max;
    line["vol_status"] = vol_status;
    line["vol_thd"] = vol_thd;
    line["cur_thd"] = cur_thd;
    line["cur_value"] = cur_value;
    line["cur_min"] = cur_min;
    line["cur_max"] = cur_max;
    line["cur_status"] = cur_status;

    line["pow_value"] = pow_value;
    line["pow_min"] = pow_min;
    line["pow_max"] = pow_max;
    line["pow_status"] = pow_status;
    line["pow_apparent"] = pow_apparent;
    line["pow_reactive"] = pow_reactive;

    line["ele_active"] = ele_active;
    line["ele_reactive"] = ele_reactive;

    line["power_factor"] = power_factor;
    line["load_rate"] = load_rate;

    line["vol_line_value"] = vol_line_value;
    line["vol_line_min"] = vol_line_min;
    line["vol_line_max"] = vol_line_max;
    line["vol_line_status"] = vol_line_status;

    m_line_item_list = line;
}

void bus_toJson::setJson(bool flag)
{
    QJsonObject json;


    if(flag){
        json["box_data"] = m_box_data;
    }
    else{
        json["bus_data"] = m_bus_data;
    }
    json["addr"] = m_addr;
    json["env_item_list"] = m_env_item_list;
    json["status"] = m_status;
    json["dev_ip"] = m_dev_ip;
    json["datetime"] = m_datetime;
    json["bar_id"] = m_bar_id;
    json["bus_name"] = m_bus_name;
    json["bus_key"] = m_bus_key;
    json["box_name"] = "";
    if(flag){
        json["box_name"] = m_box_name;
        json["box_key"] = m_box_key ;
    }

   // qDebug()<<m_box_key;
    m_json = json;
}

void bus_toJson::setBoxData()
{

    QJsonObject box;
    box["box_cfg"] = m_box_cfg;
    box["loop_item_list"] = m_loop_item_list;
    box["line_item_list"] = m_box_line_item;
    box["outlet_item_list"] = m_outlet_item_list;
    box["box_total_data"] = m_box_total_data;

    m_box_data = box;
}

void bus_toJson::setBoxTotal(Busbar& data)
{
    QJsonObject total;
    auto &u = data.boxData.boxTotalData;

    total["pow_active"] = qRound(u.powActive * 1000) / 1000.0;
    total["pow_apparent"] = qRound(u.powApparent * 1000) / 1000.0;
    total["pow_reactive"] = qRound(u.powReactive * 1000) / 1000.0;
    total["power_factor"] = qRound(u.powerFactor * 1000) / 1000.0;

    total["ele_active"] = qRound(u.eleActive * 1) / 1.0;
    total["ele_apparent"] = qRound(u.eleApparent * 1) / 1.0;
    total["ele_reactive"] = qRound(u.eleReactive * 1) / 1.0;

    m_box_total_data = total;

}

void bus_toJson::setOutLet(Busbar& data)
{
    QJsonArray pow_active,pow_apparent,pow_reactive,ele_active,ele_apparent
             ,ele_reactive,power_factor;
    QJsonObject outlet;
    auto &u = data.boxData.outletItemList;

    for(int i = 0; i < data.boxData.outletItemList.eleActive.size(); i++) {
        pow_active.append(qRound(u.powActive[i] * 1000) / 1000.0);      // 有功功率保留3位小数
        pow_apparent.append(qRound(u.powApparent[i] * 1000) / 1000.0);  // 视在功率保留3位小数
        pow_reactive.append(qRound(u.powReactive[i] * 1000) / 1000.0);  // 无功功率保留3位小数

        ele_active.append(qRound(u.eleActive[i] * 1) / 1.0);             // 有功电能保留整数
        ele_apparent.append(qRound(u.eleApparent[i] * 1) / 1.0);         // 视在电能保留整数
        ele_reactive.append(qRound(u.eleReactive[i] * 1) / 1.0);         // 无功电能保留整数

        power_factor.append(qRound(u.powerFactor[i] * 100) / 100.0);  // 功率因数保留3位小数（建议）

    }

    outlet["pow_active"] = pow_active;
    outlet["pow_apparent"] = pow_apparent;
    outlet["pow_reactive"] = pow_reactive;
    outlet["ele_active"] = ele_active;
    outlet["ele_apparent"] = ele_apparent;
    outlet["ele_reactive"] = ele_reactive;
    outlet["power_factor"] = power_factor;

    m_outlet_item_list = outlet;
}

void bus_toJson::setBoxLine(Busbar& data)
{
    QJsonArray vol_value,cur_value,cur_thd,pow_active,pow_apparent,pow_reactive
        ,load_rate,ele_active,ele_reactive,power_factor;

    QJsonObject line;
    auto &u = data.boxData.lineItemList;
    for(int i = 0; i < 3; i ++){
        vol_value.append(u.volValue[i]);
        cur_value.append(qRound(u.curValue[i] * 100) / 100.0);                // 电流保留2位小数
        cur_thd.append(u.curThd[i]);
        pow_active.append(qRound(u.powActive[i] * 1000) / 1000.0);            // 有功功率保留3位小数
        pow_apparent.append(qRound(u.powApparent[i] * 1000) / 1000.0);        // 视在功率保留3位小数
        pow_reactive.append(qRound(u.powReactive[i] * 1000) / 1000.0);        // 无功功率保留3位小数
        load_rate.append(qRound(u.loadRate[i] * 100) / 100.0);                // 负载率保留2位小数
        ele_active.append(qRound(u.eleActive[i] * 1) / 1.0);                  // 电能取整
        ele_reactive.append(qRound(u.eleReactive[i] * 1) / 1.0);              // 电能取整
        power_factor.append(qRound(u.powerFactor[i] * 100) / 100.0);

    }
    line["vol_value"] = vol_value;
    line["cur_value"] = cur_value;
    line["cur_thd"] = cur_thd;
    line["pow_active"] = pow_active;
    line["pow_apparent"] = pow_apparent;
    line["pow_reactive"] = pow_reactive;
    line["load_rate"] = load_rate;
    line["ele_active"] = ele_active;
    line["ele_reactive"] = ele_reactive;
    line["power_factor"] = power_factor;

    m_box_line_item = line;
    //qDebug()<<line;
}

void bus_toJson::setLoop(Busbar& data)
{
    QJsonArray vol_value,vol_min,vol_max,vol_status,cur_value,cur_min
                ,cur_max,cur_status,pow_value,pow_min,pow_max,pow_status,
        pow_reactive,pow_apparent,power_factor,ele_active,ele_reactive;

    QJsonObject loop;

    auto &u = data.boxData.loopItemList;

    for(int i = 0; i < data.boxData.boxCfg.loopNum; i ++){
        vol_value.append(u.volValue[i]);
        vol_min.append(u.volMin[i]);
        vol_max.append(u.volMax[i]);

        if(u.volStatus[i])st = 1;
        vol_status.append(u.volStatus[i]);
        cur_value.append(qRound(u.curValue[i] * 100) / 100.0);  // 电流保留 2 位小数

        cur_min.append(u.curMin[i]);
        cur_max.append(u.curMax[i]);
        if(u.curStatus[i])st = 1;
        cur_status.append(u.curStatus[i]);
        pow_value.append(qRound(u.powValue[i] * 1000) / 1000.0);  // 保留3位小数

        pow_min.append(u.powMin[i]);
        pow_max.append(u.powMax[i]);
        if(u.powStatus[i])st = 1;
        pow_status.append(u.powStatus[i]);
        pow_reactive.append(qRound(u.powReactive[i] * 1000) / 1000.0);     // 保留3位小数
        pow_apparent.append(qRound(u.powApparent[i] * 1000) / 1000.0);     // 保留3位小数
        power_factor.append(qRound(u.powerFactor[i] * 100) / 100.0);     // 保留3位小数

        ele_active.append(qRound(u.eleActive[i]*1)/1.0);
        ele_reactive.append(qRound(u.eleReactive[i])/1.0);
    }

    loop["vol_value"] = vol_value;
    loop["vol_min"] = vol_min;
    loop["vol_max"] = vol_max;
    loop["vol_status"] = vol_status;
    loop["cur_value"] = cur_value;
    loop["cur_min"] = cur_min;
    loop["cur_max"] = cur_max;
    loop["cur_status"] = cur_status;
    loop["pow_value"] = pow_value;
    loop["pow_min"] = pow_min;
    loop["pow_max"] = pow_max;
    loop["pow_status"] = pow_status;
    loop["pow_reactive"] = pow_reactive;
    loop["pow_apparent"] = pow_apparent;
    loop["power_factor"] = power_factor;
    loop["ele_active"] = ele_active;
    loop["ele_reactive"] = ele_reactive;

    m_loop_item_list = loop;

}

void bus_toJson::setBoxCfg(Busbar& data)
{
    auto &u = data.boxData.boxCfg;
    QJsonObject cfg;
    cfg["alarm_count"] = u.alarmCount;
    cfg["iof"] = u.iof;
    cfg["box_version"] = u.boxVersion;
    cfg["baud_rate"] = u.baudRate;
    cfg["beep"] = u.beep;
    cfg["item_type"] = u.itemType;
    cfg["work_mode"] = u.workMode;
    cfg["loop_num"] = u.loopNum;
    cfg["box_type"] = u.boxType;

    QJsonArray status;
    for(int i = 0; i < u.loopNum; i++){
        status.append(u.breakerStatus[i]);
    }
    cfg["breaker_status"] = status;
    m_box_cfg = cfg;

}

void bus_toJson::setTem(Busbar& Data, bool flag)
{
    QJsonObject env;
    auto &u = Data.envItemList;

    QVector<int> tem_min(4,0), tem_max(4,0), tem_status(4,0);
    QVector<double>tem_value(4,0);

    QJsonArray tem,max,min,status;

    for(int i = 0; i < 4; i++){
        tem_min[i] = 0;
        tem_max[i] = 100;
        tem_status[i] = 0;
        if(flag)tem_value[i] = u.boxtem[i];
        else tem_value[i] = u.temValue[i];
        tem.append(tem_value[i]);
        min.append(tem_min[i]);
        max.append(tem_max[i]);
        status.append(tem_status[i]);
    }
    env["tem_min"] = min;
    env["tem_max"] = max;
    env["tem_status"] = status;
    env["tem_value"] = tem;

    m_env_item_list = env;


}

void bus_toJson::setGlobalData(Busbar& Data)
{
    auto &u = Data.info;
    m_addr = u.addr;

    if(st)m_status = 2;
    else m_status = 1;
    m_dev_ip = u.devIp;
    m_datetime = u.datetime;
    m_bar_id = u.barId;
    m_bus_name = u.busName;
    m_box_name = u.boxName;
    m_bus_key = u.busKey;
    m_box_key = u.boxKey;
}
