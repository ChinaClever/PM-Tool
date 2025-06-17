#include "mp_tojson.h"
#include <QDebug>
#include <QDateTime>

#include <math.h>

mp_toJson::mp_toJson(QObject *parent) : QObject(parent)
{

}


void mp_toJson::toJsonSlot(PowerSystemData systemData)
{
   // debugPrintSystemData();

    setGlobalData(systemData);
    setEnvItemList(systemData);
    setOutBitItemList(systemData);
    setLoopItemList(systemData);
    setLineItemList(systemData);
    setPduTotalData(systemData);
    generatePduData(systemData);
    generateJson();
}

QJsonObject mp_toJson::generateJson()
{
    QJsonObject json;
    json["addr"] = m_addr;
    json["status"] = m_status;
    json["dev_hz"] = m_devhz;
    json["dev_key"] = m_dev_key;
    json["dev_ip"] = m_dev_ip;
    json["datetime"] = m_datetime;
    json["pdu_data"] = m_pduData;


    QJsonObject info;   //
    info["pdu_type"] = "MPDU Pro";
    json["uut_info"] = info;


    m_json = json;
    return json;

}

QJsonObject mp_toJson::getJsonObject() const
{
    return m_json;
}

void mp_toJson::generatePduData(PowerSystemData systemData)
{
    QJsonObject pduItem;

    pduItem["env_item_list"] = m_env_item_list;
    pduItem["line_item_list"] = m_line_item_list;
    pduItem["loop_item_list"] = m_loop_item_list;
    if(systemData.settings.series != "A")
    pduItem["output_item_list"] = m_output_item_list;
    pduItem["pdu_total_data"] = m_pdu_total_data;

    m_pduData = pduItem;
}

void mp_toJson::setPduTotalData(PowerSystemData systemData)
{
    QJsonObject totalItem;
    int cnt = systemData.pduData.phases.phaseNum;

    // 直接存储单个值到JSON对象
    totalItem["ele_active"] = qRound(systemData.pduData.totalData.eleActive*1000)/1000.0;
    totalItem["pow_apparent"] = qRound(systemData.pduData.totalData.powApparent*1000)/1000.0;
    totalItem["pow_active"] = qRound(systemData.pduData.totalData.powActive*1000)/1000.0;
    totalItem["pow_reactive"] = qRound(systemData.pduData.totalData.powReactive*1000)/1000.0;
    totalItem["power_factor"] = qRound(systemData.pduData.totalData.powerFactor*100)/100.0;
    if(cnt == 3){
        totalItem["cur_unbalance"] = qRound(systemData.pduData.totalData.curUnbalance*100)/100.0;
        totalItem["vol_unbalance"] = qRound(systemData.pduData.totalData.volUnbalance*100)/100.0;
    }
    m_pdu_total_data = totalItem;


}

void mp_toJson::setLineItemList(PowerSystemData systemData)
{
    QJsonObject lineItem;
    int cnt = systemData.pduData.phases.phaseNum;

    // 原始数据容器
    QVector<int> cur_alarm_status(cnt,0), pow_alarm_status(cnt,0), vol_alarm_status(cnt,0);
    QVector<double> cur_alarm_max(cnt,0.0), cur_value(cnt,0.0), ele_active(cnt,0.0),
        pow_reactive(cnt,0.0), pow_apparent(cnt,0.0), vol_value(cnt,0.0),
        pow_value(cnt,0.0), power_factor(cnt,0.0), phase_voltage(cnt,0.0);

    // JSON数组容器
    QJsonArray curAlarmStatusArr, powAlarmStatusArr, volAlarmStatusArr,
        curAlarmMaxArr, curValueArr, eleActiveArr, powReactiveArr,
        powApparentArr, volValueArr, powValueArr, powerFactorArr,
        phaseVoltageArr;

    for(int i=0; i<cnt; i++){
        // 原始数据采集逻辑
        vol_value[i] = systemData.pduData.phases.voltages[i];
        cur_value[i] = qRound(systemData.pduData.phases.currents[i]*100)/100.0;
        cur_alarm_max[i] = systemData.thresholds.capPhaseCur;
        cur_alarm_status[i] = cur_value[i] > cur_alarm_max[i] ? 8 : 0;
        if(cur_alarm_status[i]) m_status = 2;
        vol_alarm_status[i] = 0;
        pow_alarm_status[i] = 0;
        pow_reactive[i] = qRound(systemData.pduData.phases.reactivePowers[i]*1000)/1000.0;
        pow_apparent[i] = qRound(systemData.pduData.phases.apparentPowers[i]*1000)/1000.0;
        pow_value[i] = qRound(systemData.pduData.phases.activePowers[i]*1000)/1000.0;
        power_factor[i] = qRound(systemData.pduData.phases.powerFactors[i]*100)/100.0;
        ele_active[i] = qRound(systemData.pduData.phases.energies[i]*1000)/1000.0;
        phase_voltage[i] = qRound(vol_value[i] / sqrt(3) *100.0)/100.0;

        // 将数据添加到JSON数组
        curAlarmStatusArr.append(cur_alarm_status[i]);
        powAlarmStatusArr.append(pow_alarm_status[i]);
        volAlarmStatusArr.append(vol_alarm_status[i]);
        curAlarmMaxArr.append(cur_alarm_max[i]);
        curValueArr.append(cur_value[i]);
        eleActiveArr.append(ele_active[i]);
        powReactiveArr.append(pow_reactive[i]);
        powApparentArr.append(pow_apparent[i]);
        volValueArr.append(vol_value[i]);
        powValueArr.append(pow_value[i]);
        powerFactorArr.append(power_factor[i]);
        phaseVoltageArr.append(phase_voltage[i]);
    }

    lineItem["cur_alarm_status"] = curAlarmStatusArr;
    lineItem["pow_alarm_status"] = powAlarmStatusArr;
    lineItem["vol_alarm_status"] = volAlarmStatusArr;
    lineItem["cur_alarm_max"] = curAlarmMaxArr;
    lineItem["cur_value"] = curValueArr;
    lineItem["ele_active"] = eleActiveArr;
    lineItem["pow_reactive"] = powReactiveArr;
    lineItem["pow_apparent"] = powApparentArr;
    lineItem["vol_value"] = volValueArr;
    lineItem["pow_value"] = powValueArr;
    lineItem["power_factor"] = powerFactorArr;
    lineItem["phase_voltage"] = phaseVoltageArr;

    m_line_item_list = lineItem;

}

void mp_toJson::setLoopItemList(PowerSystemData systemData)//回路信息
{
    QJsonObject LoopItem;

    int cnt = systemData.settings.circuitCount;

    QVector<int> breaker(cnt, 0), cur_alarm_status(cnt,0), pow_alarm_status(cnt,0.0), vol_alarm_status(cnt,0);
    QVector<double>cur_alarm_max(cnt,0.0), cur_value(cnt,0.0), ele_active(cnt,0.0), pow_reactive(cnt,0.0), pow_apparent(cnt,0.0)
        , vol_value(cnt,0.0), pow_value(cnt,0.0), power_factor(cnt,0.0) ;

    QJsonArray breakerArr, curAlarmStatusArr, powAlarmStatusArr, volAlarmStatusArr,
        curAlarmMaxArr, curValueArr, eleActiveArr, powReactiveArr,
        powApparentArr, volValueArr, powValueArr, powerFactorArr;

    for(int i = 0;i < cnt;i ++){
        ele_active[i] = qRound(systemData.pduData.loopData.Circuits[i].energy *1000.0)/1000.0;
        breaker[i] = systemData.pduData.loopData.Circuits[i].breakerStatus;
        if(breaker[i]){
            cur_alarm_status[i] = systemData.pduData.loopData.Circuits[i].curAlarmStatus;

            if(cur_alarm_status[i] == 8) m_status = 2;

            pow_alarm_status[i] = systemData.pduData.loopData.Circuits[i].powAlarmStatus;
            pow_apparent[i] = qRound(systemData.pduData.loopData.Circuits[i].apparentPower*1000)/1000.0;
            vol_alarm_status[i] = systemData.pduData.loopData.Circuits[i].volAlarmStatus;
            cur_alarm_max[i] = systemData.pduData.loopData.Circuits[i].curAlarmMax;
            cur_value[i] = qRound(systemData.pduData.loopData.Circuits[i].current*100)/100.0;
            pow_reactive[i] = qRound(systemData.pduData.loopData.Circuits[i].reactivePower*1000)/1000.0;
            vol_value[i] = qRound(systemData.pduData.loopData.Circuits[i].voltage*100)/100.0;
            pow_value[i] = qRound(systemData.pduData.loopData.Circuits[i].activePower*1000)/1000.0;
            power_factor[i] = qRound(systemData.pduData.loopData.Circuits[i].powerFactor*100)/100.0;
        }
        breakerArr.append(breaker[i]);
        curAlarmStatusArr.append(cur_alarm_status[i]);
        powAlarmStatusArr.append(pow_alarm_status[i]);
        volAlarmStatusArr.append(vol_alarm_status[i]);
        curAlarmMaxArr.append(cur_alarm_max[i]);
        curValueArr.append(cur_value[i]);
        eleActiveArr.append(ele_active[i]);
        powReactiveArr.append(pow_reactive[i]);
        powApparentArr.append(pow_apparent[i]);
        volValueArr.append(vol_value[i]);
        powValueArr.append(pow_value[i]);
        powerFactorArr.append(power_factor[i]);
    }
    LoopItem["breaker"] = breakerArr;
    LoopItem["cur_alarm_status"] = curAlarmStatusArr;
    LoopItem["pow_alarm_status"] = powAlarmStatusArr;
    LoopItem["vol_alarm_status"] = volAlarmStatusArr;
    LoopItem["cur_alarm_max"] = curAlarmMaxArr;
    LoopItem["cur_value"] = curValueArr;
    LoopItem["ele_active"] = eleActiveArr;
    LoopItem["pow_reactive"] = powReactiveArr;
    LoopItem["pow_apparent"] = powApparentArr;
    LoopItem["vol_value"] = volValueArr;
    LoopItem["pow_value"] = powValueArr;
    LoopItem["power_factor"] = powerFactorArr;

    m_loop_item_list = LoopItem;

}

void mp_toJson::setOutBitItemList(PowerSystemData systemData)//输出位信息
{
    QJsonObject BitItem;

    int cnt = systemData.settings.outputBitCount;

    QVector<int> relay_state(cnt, 0), pow_alarm_status(cnt,0), cur_alarm_status(cnt,0.0);
    QVector<double>cur_value(cnt,0.0), power_factor(cnt,0.0), pow_value(cnt,0.0), pow_reactive(cnt,0.0)
        , pow_apparent(cnt,0.0), ele_active(cnt,0.0), cur_alarm_max(cnt,0.0);

    QJsonArray relayArray, curValueArray, powerFactorArray;
    QJsonArray powValueArray, powReactiveArray, powApparentArray, eleActiveArray;
    QJsonArray powAlarmStatusArray, curAlarmStatusArray, curAlarmMaxArray;

    for (int i = 0; i < cnt; ++i) {
        ele_active[i] = qRound(systemData.pduData.outputData.outputBits[i].energy *100.0)/100.0;
        relay_state[i] = systemData.pduData.outputData.outputBits[i].switchStatus;

        if(relay_state[i]){
            cur_value[i] = qRound(systemData.pduData.outputData.outputBits[i].current * 100.0)/100.0;
            power_factor[i] = qRound(systemData.pduData.outputData.outputBits[i].powerFactor*100)/100.0;
            pow_value[i] = qRound(systemData.pduData.outputData.outputBits[i].activePower*1000)/1000.0;
            pow_reactive[i] = qRound(systemData.pduData.outputData.outputBits[i].reactivePower*1000)/1000.0;
            pow_apparent[i] = qRound(systemData.pduData.outputData.outputBits[i].apparentPower*1000)/1000.0;
            pow_alarm_status[i] = systemData.pduData.outputData.outputBits[i].powAlarmStatus;
            cur_alarm_status[i] = systemData.pduData.outputData.outputBits[i].curAlarmStatus;
            cur_alarm_max[i] = systemData.pduData.outputData.outputBits[i].curAlarmMax;
        }
        relayArray.append(relay_state[i]);
        curValueArray.append(cur_value[i]);
        powerFactorArray.append(power_factor[i]);
        powValueArray.append(pow_value[i]);
        powReactiveArray.append(pow_reactive[i]);
        powApparentArray.append(pow_apparent[i]);
        eleActiveArray.append(ele_active[i]);
        powAlarmStatusArray.append(pow_alarm_status[i]);
        curAlarmMaxArray.append(cur_alarm_max[i]);
        curAlarmStatusArray.append(cur_alarm_status[i]);
    }

    QString series = systemData.settings.series;

    if(series == "C" || series == "D"){
        BitItem["relay_state"] = relayArray;
    }
    if(series == "B" || series == "D"){
        BitItem["cur_value"] = curValueArray;
        BitItem["power_factor"] = powerFactorArray;
        BitItem["pow_value"] = powValueArray;
        BitItem["pow_reactive"] = powReactiveArray;
        BitItem["pow_apparent"] = powApparentArray;
        BitItem["ele_active"] = eleActiveArray;
        BitItem["pow_alarm_status"] = powAlarmStatusArray;
        BitItem["cur_alarm_max"] = curAlarmMaxArray;
        BitItem["cur_alarm_status"] = curAlarmStatusArray;
    }
    m_output_item_list = BitItem;
}

void mp_toJson::setEnvItemList(PowerSystemData systemData) //环境信息
{
    QJsonObject envItem;

    int insert[4]={0};
    double hum[4] ={0};
    double tem[4] ={0};
    int temAlarm[4] = {0};
    int humAlarm[4] = {0};
    double dewPoint[4] = {0};
    for(int i = 0;i < 4;i ++){
        insert[i] = systemData.pduData.envData.sensorStatuses[i];
        hum[i] = qRound(systemData.pduData.envData.humidities[i]*1)/1.0;
        tem[i] = qRound(systemData.pduData.envData.temperatures[i]*10)/10.0;
        temAlarm[i] = systemData.pduData.envData.temAlarm[i];
        humAlarm[i] = systemData.pduData.envData.hunAlarm[i];
        dewPoint[i] = qRound(systemData.pduData.envData.dewPoints[i]*100)/100.0;
    }

    envItem["insert"] = QJsonArray::fromVariantList({insert[0], insert[1], insert[2], insert[3]});
    envItem["hum_value"] = QJsonArray::fromVariantList({hum[0], hum[1], hum[2], hum[3]});
    envItem["tem_value"] = QJsonArray::fromVariantList({tem[0], tem[1], tem[2], tem[3]});
    envItem["dew_point"] = QJsonArray::fromVariantList({dewPoint[0], dewPoint[1], dewPoint[2], dewPoint[3]});
    envItem["hum_alarm_status"] = QJsonArray::fromVariantList({humAlarm[0], humAlarm[1], humAlarm[2], humAlarm[3]});
    envItem["tem_alarm_status"] = QJsonArray::fromVariantList({temAlarm[0], temAlarm[1], temAlarm[2], temAlarm[3]});

    m_env_item_list = envItem;

}

void mp_toJson::setGlobalData(PowerSystemData systemData) //基础信息
{
    m_addr = systemData.addr;
    m_status = systemData.status;
    m_version = systemData.version;
    m_dev_ip = systemData.devIp;
    m_datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    setDevkey();
}

void mp_toJson::setDevkey()
{
    QString dev_key = m_dev_ip + "-" + QString("%1").arg(m_addr, 1, 10, QChar('0'));
    m_dev_key = dev_key;
}
