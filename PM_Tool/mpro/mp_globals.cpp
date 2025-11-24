#include "mp_globals.h"
#include "specrannumggen.h"
#include "data_cal/data_cal.h"
#include "databasemanager.h"
#include <QDebug>
#include <QDateTime>
#include <math.h>
#include <QQueue>            // QQueue 定义
#include <QVector>           // QVector 定义

const int QUEUE_COUNT = 4;

PowerSystemData m_systemData;

int mp_sendTime;

QVector<QQueue<QJsonObject>> ProBulkJQs(QUEUE_COUNT);
QMutex ProBulkJQMutexes[QUEUE_COUNT];

QMap<QString, PowerSystemData> AMap;
QMap<QString, PowerSystemData> BMap;
QMap<QString, PowerSystemData> CMap;
QMap<QString, PowerSystemData> DMap;

int MpCnt;
int MpCntt;
int MpCntEr;

int Anum,Bnum,Cnum,Dnum;


void envInc(PowerSystemData& u) //环境增量改变
{
    int size = u.pduData.envData.dewPoints.size();

    for(int i = 0; i < size; ++i){
        //温度
        double x = specRanNumGgen::getrandom(100);
        double &y = u.pduData.envData.temperatures[i];
        double newVal = u.pduData.envData.incr[i] ?x+y : y-x;

        y = ((u.pduData.envData.incr[i] ? fmin(x + y, 45) : fmax(y - x, 20)));
        u.pduData.envData.incr[i] = (u.pduData.envData.incr[i] && newVal >= 45) ? false :
                            (!u.pduData.envData.incr[i] && newVal <= 20)   ? true  : u.pduData.envData.incr[i];

        //湿度
        x = specRanNumGgen::getrandom(100);
        double &yy = u.pduData.envData.humidities[i];
        newVal = u.pduData.envData.humincr[i] ?x+yy : yy-x;
        yy = ((u.pduData.envData.humincr[i] ? fmin(x + yy, 100) : fmax(yy - x, 0)));
        u.pduData.envData.humincr[i] = (u.pduData.envData.humincr[i] && newVal >= 100) ? false :
                             (!u.pduData.envData.humincr[i] && newVal <= 1)   ? true  : u.pduData.envData.humincr[i];

        u.pduData.envData.dewPoints[i] = data_cal::calculate_dewpoint1(u.pduData.envData.temperatures[i], u.pduData.envData.humidities[i]);
    }

    int x = specRanNumGgen::get_power_factor_precise() * 10;
    int y = specRanNumGgen::get_power_factor_precise() * 10;

    if(x == 2) x = 2;else x = 1;
    if(y == 2) y = 2;else y = 1;

   //qDebug()<<x<<" "<<y;

    u.pduData.envData.door[0] = u.pduData.envData.door[1] = x;
    u.pduData.envData.lock[0] = y;u.pduData.envData.water[0] = y;
    u.pduData.envData.smoke[0] = x;

    //qDebug()<<u.pduData.envData.temperatures[0]<<"   "<<u.pduData.envData.humidities[0];
}

void bitInc(PowerSystemData& u) //输出位增量变化
{
    int size = u.pduData.outputData.outputBits.size();
    for(int i = 0; i < size; ++i){
        double x = specRanNumGgen::getrandom(25);
        double &y = u.pduData.outputData.outputBits[i].current;
        auto &incr = u.pduData.outputData.outputBits[i].curinc;
        double newVal = incr ?x+y : y-x;
        u.pduData.outputData.outputBits[i].current = (incr ? fmin(x + y, 8) : fmax(y - x, 0));

        incr = (incr && newVal >= 8) ? false :(!incr && newVal <= 0)   ? true  : incr;

        x = specRanNumGgen::getrandom(8);
        double &yy = u.pduData.outputData.outputBits[i].powerFactor;
        auto &pfincr = u.pduData.outputData.outputBits[i].pfinc;
        newVal = pfincr ? x+yy : yy-x;
        yy = (pfincr ? fmin(x + yy, 1) : fmax(yy - x, 0));
        pfincr = (pfincr && newVal >= 1) ? false :(!pfincr && newVal <= 0)   ? true  : pfincr;

        u.pduData.outputData.outputBits[i].activePower = data_cal::active_powerCal(u.pduData.outputData.outputBits[i].vol,u.pduData.outputData.outputBits[i].current,u.pduData.outputData.outputBits[i].powerFactor);
        u.pduData.outputData.outputBits[i].apparentPower = data_cal::apparent_powerCal(u.pduData.outputData.outputBits[i].vol,u.pduData.outputData.outputBits[i].current);
        u.pduData.outputData.outputBits[i].reactivePower = data_cal::reactive_powerCal(u.pduData.outputData.outputBits[i].apparentPower,u.pduData.outputData.outputBits[i].activePower);

        u.pduData.outputData.outputBits[i].energy +=
            (u.pduData.outputData.outputBits[i].activePower * mp_sendTime)/3600;
    }

    // {
    //     auto key = u.dev_key;
    //     int size = u.pduData.outputData.outputBits.size();
    //     double energies[48] = {0};
    //     for(int i = 0; i < size; ++i){
    //         energies[i] = u.pduData.outputData.outputBits[i].energy;
    //     }
    //     DatabaseManager::instance().insertOrUpdateOutputBitEnergy(key, energies);
    // }
}

void debugPrintSystemData(PowerSystemData& packet)
{
    qDebug() << "\n\n===== 电力系统数据详细调试信息 =====";
    qDebug() << "======= " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << " =======";

    // 1. 设备基本信息
    qDebug() << "\n[设备基本信息]";
    qDebug() << "地址:" << packet.addr;
    qDebug() << "状态:" << packet.status;
    qDebug() << "设备IP:" << packet.devIp;
    qDebug() << "设备频率:" << packet.devHz << "Hz";
    qDebug() << "版本:" << packet.version;
    qDebug() << "日期时间:" << packet.datetime;

    // 2. 环境数据
    qDebug() << "\n[环境监测数据]";
    qDebug() << "温度(℃)\t湿度(%)\t状态\t露点温度(℃)";
    for(int i = 0; i < 4; i++) {
        qDebug() << QString("  %1       %2        %3          %4")
        .arg(packet.pduData.envData.temperatures[i], 0, 'f', 1)
            .arg(packet.pduData.envData.humidities[i], 0, 'f', 1)
            .arg(packet.pduData.envData.sensorStatuses[i] ? "启用" : "禁用")
            .arg(packet.pduData.envData.dewPoints[i], 0, 'f', 1);
    }

    // 3. 输出位详细数据
    qDebug() << "\n[输出位数据 - 共" << packet.pduData.outputData.outputBits.size() << "个]";
    qDebug() << "ID\t开关\t电流(A)\t有功(W)\t无功(var)\t视在(VA)\t功率因数\t电能(kWh)\t电流增量";
    for(int i = 0; i < packet.pduData.outputData.outputBits.size(); i++) {
        const auto& bit = packet.pduData.outputData.outputBits[i];
        qDebug() << QString("%1 %2 %3 %4 %5 %6 %7 %8 %9")
                        .arg(i+1)
                        .arg(bit.switchStatus ? "开" : "关")
                        .arg(bit.current, 0, 'f', 3)
                        .arg(bit.activePower, 0, 'f', 3)
                        .arg(bit.reactivePower, 0, 'f', 3)
                        .arg(bit.apparentPower, 0, 'f', 3)
                        .arg(bit.powerFactor, 0, 'f', 2)
                        .arg(bit.energy, 0, 'f', 3)
                        .arg(bit.currentIncrement, 0, 'f', 3);
    }

    // 4. 回路详细数据
    qDebug() << "\n[回路数据 - 共" << packet.pduData.loopData.Circuits.size() << "个]";
    qDebug() << "ID\t断路器\t电压(V)\t电流(A)\t有功(W)\t无功(var)\t视在(VA)\t功率因数\t电能(kWh)";
    for(int i = 0; i < packet.pduData.loopData.Circuits.size(); i++) {
        qDebug() <<packet.pduData.loopData.Circuits[i].current;

    }

    // 5. 相位详细数据
    bool isSinglePhase = (packet.pduData.phases.phaseNum == 1);
    qDebug() << "\n[相位数据 - " << (isSinglePhase ? "单相" : "三相") << "]";
    qDebug() << "相位\t电压(V)\t电流(A)\t有功(W)\t无功(var)\t视在(VA)\t功率因数\t电能(kWh)";

    auto printPhaseData = [](const QString& phaseName, const PhaseData& data, int index) {
        qDebug() << QString("%1 %2 %3 %4 %5 %6 %7 %8")
        .arg(phaseName)
            .arg(data.voltages.value(index, 0.0), 0, 'f', 1)
            .arg(data.currents.value(index, 0.0), 0, 'f', 3)
            .arg(data.activePowers.value(index, 0.0), 0, 'f', 3)
            .arg(data.reactivePowers.value(index, 0.0), 0, 'f', 3)
            .arg(data.apparentPowers.value(index, 0.0), 0, 'f', 3)
            .arg(data.powerFactors.value(index, 0.0), 0, 'f', 2)
            .arg(data.energies.value(index, 0.0), 0, 'f', 3);
    };

    printPhaseData("A相", packet.pduData.phases, 0);
    if(!isSinglePhase) {
        printPhaseData("B相", packet.pduData.phases, 1);
        printPhaseData("C相", packet.pduData.phases, 2);
    }

    // 6. 全局统计数据
    qDebug() << "\n[全局统计数据]";
    qDebug() << "总有功功率:" << packet.pduData.totalData.powActive << "W";
    qDebug() << "总无功功率:" << packet.pduData.totalData.powReactive << "var";
    qDebug() << "总视在功率:" << packet.pduData.totalData.powApparent << "VA";
    qDebug() << "总功率因数:" << packet.pduData.totalData.powerFactor;
    qDebug() << "总电能:" << packet.pduData.totalData.eleActive << "kWh";
    qDebug() << "电压不平衡度:" << packet.pduData.totalData.volUnbalance << "%";
    qDebug() << "电流不平衡度:" << packet.pduData.totalData.curUnbalance << "%";

    // 7. 系统设置
    qDebug() << "\n[系统设置]";
    qDebug() << "系列:" << packet.settings.series;
    qDebug() << "相位模式:" << packet.settings.phaseMode;
    qDebug() << "输出位数量:" << packet.settings.outputBitCount;
    qDebug() << "回路数量:" << packet.settings.circuitCount;

    // 8. 阈值设置
    qDebug() << "\n[阈值设置]";
    qDebug() << "电压阈值:" << packet.thresholds.capVol << "V";
    qDebug() << "回路电流阈值:" << packet.thresholds.capCirCur << "A";
    qDebug() << "相位电流阈值:" << packet.thresholds.capPhaseCur << "A";
    qDebug() << "输出位电流阈值:" << packet.thresholds.capbitCur << "A";

    qDebug() << "=============================================\n";
}
