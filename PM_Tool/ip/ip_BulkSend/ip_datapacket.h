#pragma once
#include <array>
#include <QDebug>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
template <int SIZE>
struct sObjLine {
    std::array<int, SIZE>cur_alarm_status = {0}; //报警状态
    std::array<int, SIZE>vol_alarm_status = {0};
    std::array<int, SIZE>pow_alarm_status = {0};

    std::array<double, SIZE> vol_value;
    std::array<double, SIZE> cur_value;
    std::array<double, SIZE> cur_alarm_max;//报警上限
    std::array<double, SIZE> vol_alarm_max;



    std::array<double, SIZE> pow_value;
    std::array<double, SIZE> pow_apparent;
    std::array<double, SIZE> pow_reactive;
    std::array<double, SIZE> power_factor; //功率因素
    std::array<double, SIZE> ele_active;  //有功电能
};

template <int SIZE>
struct sObjEnv {
    std::array<int, 1> insert{};
    std::array<double, 1> dew_point{};
    std::array<double, 1> hum_value{};
    std::array<double, 1> tem_value{};
    std::array<int, 1> hum_alarm_status{};
    std::array<int, 1> tem_alarm_status{};
};

struct sObjTotal {
    double cur_rms{};
    double vol_rms{};
    double ele_active{};    //有功电能
    double power_factor{}; //功率因素
    double pow_active{};    //总有功功率
    double pow_apparent{};  //总视在功率
    double pow_reactive{};  //总无功功率
    double vol_unbalance{}; //不平衡度
    double cur_unbalance{};
};

template <int SIZE>
struct IP_sDataPacket {


    bool statusflag = true;

    bool incrEnvInc = true; //温度增量方向
    bool incrEnvHum = true;//湿度

    bool volIncA = true;//A相电压
    bool volIncB = true;
    bool volIncC = true;

    bool curIncA = true;//A相电流
    bool curIncB = true;
    bool curIncC = true;

    bool pfIncA = true; //A相功率因素
    bool pfIncB = true;
    bool pfIncC = true;

    bool eleInc = true; //电能


    int Timesend = 10;

    int addr{};
    int status{};
    int version{};

    QString dev_ip;
    QString dev_key;
    QString datetime;
    QString pdu_alarm;

    sObjEnv<SIZE> env_item;
    sObjLine<SIZE> line_item;
    sObjTotal total_item;

    void setBasicParams(int Addr, QString devip, QString key) {
        addr = Addr;
        dev_ip = devip;
        dev_key= key;
    }

    static IP_sDataPacket<SIZE>create() {
        IP_sDataPacket<SIZE> packet;

        packet.statusflag = true;

        packet.incrEnvInc = true; //温度增量方向
        packet.incrEnvHum = true;//湿度

        packet.volIncA = true;//A相电压
        packet.volIncB = true;
        packet.volIncC = true;

        packet.curIncA = true;//A相电流
        packet.curIncB = true;
        packet.curIncC = true;

        packet.pfIncA = true; //A相功率因素
        packet.pfIncB = true;
        packet.pfIncC = true;

        packet.eleInc = true; //电能

        packet.status = 0;
        packet.version = 1;
        packet.pdu_alarm = "功能正常";


        packet.env_item.insert[0]= 1;
        packet.env_item.hum_alarm_status[0] = 0;
        packet.env_item.tem_alarm_status[0] = 0;


        packet.line_item.ele_active = {0}; //有功电能初始为0
        for(int i=0;i<SIZE;i++)packet.line_item.vol_alarm_max[i] = {276};

        packet.total_item.cur_rms = {0};
        packet.total_item.vol_rms = {0};
        packet.total_item.ele_active={0};
        packet.total_item.power_factor={0};
        packet.total_item.pow_active={0};
        packet.total_item.pow_apparent={0};
        packet.total_item.pow_reactive={0};
        packet.total_item.vol_unbalance={0};
        packet.total_item.cur_unbalance={0};

        packet.total_item = sObjTotal{}; // 全部默认值

        return packet;
    }
    void totalDataCal();
    void intiline(const int curCap);
};


// ... 其他定义 ...

template <int SIZE>
QDebug operator<<(QDebug debug, const IP_sDataPacket<SIZE>& packet) {
    debug.nospace() << "IP_sDataPacket<" << SIZE << "> {\n"
                    << "  addr: " << packet.addr << ",\n"
                    << "  status: " << packet.status << ",\n"
                    << "  version: " << packet.version << ",\n"
                    << "  dev_ip: \"" << packet.dev_ip << "\",\n"
                    << "  dev_key: \"" << packet.dev_key << "\",\n"
                    << "  datetime: \"" << packet.datetime << "\",\n"
                    << "  pdu_alarm: \"" << packet.pdu_alarm << "\",\n"
                    << "  env_item: {\n"
                    << "    insert: [" << packet.env_item.insert[0] << "],\n"
                    << "    hum_value: [" << packet.env_item.hum_value[0] << "],\n"
                    << "    tem_value: [" << packet.env_item.tem_value[0] << "],\n"
                    << "    dew_point: [" << packet.env_item.dew_point[0] << "],\n"
                    << "    hum_alarm_status: [" << packet.env_item.hum_alarm_status[0] << "],\n"
                    << "    tem_alarm_status: [" << packet.env_item.tem_alarm_status[0] << "]\n"
                    << "  },\n"
                    << "  line_item: {\n"
                    << "    cur_alarm_status: " << QJsonArray::fromVariantList(QVariantList(packet.line_item.cur_alarm_status.begin(), packet.line_item.cur_alarm_status.end())) << ",\n"
                    << "    vol_alarm_status: " << QJsonArray::fromVariantList(QVariantList(packet.line_item.vol_alarm_status.begin(), packet.line_item.vol_alarm_status.end())) << ",\n"
                    << "    vol_value: " << QJsonArray::fromVariantList(QVariantList(packet.line_item.vol_value.begin(), packet.line_item.vol_value.end())) << ",\n"
                    << "    cur_value: " << QJsonArray::fromVariantList(QVariantList(packet.line_item.cur_value.begin(), packet.line_item.cur_value.end())) << ",\n"
                    << "    cur_alarm_max: " << QJsonArray::fromVariantList(QVariantList(packet.line_item.cur_alarm_max.begin(), packet.line_item.cur_alarm_max.end())) << ",\n"
                    << "    pow_value: " << QJsonArray::fromVariantList(QVariantList(packet.line_item.pow_value.begin(), packet.line_item.pow_value.end())) << ",\n"
                    << "    pow_apparent: " << QJsonArray::fromVariantList(QVariantList(packet.line_item.pow_apparent.begin(), packet.line_item.pow_apparent.end())) << ",\n"
                    << "    pow_reactive: " << QJsonArray::fromVariantList(QVariantList(packet.line_item.pow_reactive.begin(), packet.line_item.pow_reactive.end())) << ",\n"
                    << "    power_factor: " << QJsonArray::fromVariantList(QVariantList(packet.line_item.power_factor.begin(), packet.line_item.power_factor.end())) << ",\n"
                    << "    ele_active: " << QJsonArray::fromVariantList(QVariantList(packet.line_item.ele_active.begin(), packet.line_item.ele_active.end())) << "\n"
                    << "  },\n"
                    << "  total_item: {\n"
                    << "    cur_rms: " << packet.total_item.cur_rms << ",\n"
                    << "    vol_rms: " << packet.total_item.vol_rms << ",\n"
                    << "    ele_active: " << packet.total_item.ele_active << ",\n"
                    << "    power_factor: " << packet.total_item.power_factor << ",\n"
                    << "    pow_active: " << packet.total_item.pow_active << ",\n"
                    << "    pow_apparent: " << packet.total_item.pow_apparent << ",\n"
                    << "    pow_reactive: " << packet.total_item.pow_reactive << ",\n"
                    << "    vol_unbalance: " << packet.total_item.vol_unbalance << ",\n"
                    << "    cur_unbalance: " << packet.total_item.cur_unbalance << "\n"
                    << "  }\n"
                    << "}";

    return debug.maybeSpace();
}
// 通用格式化函数（针对std::array）
template<size_t N>
QJsonArray formatToThreeDecimals(const std::array<double, N>& container) {
    QJsonArray arr;
    for (double val : container) {
        // 使用 qRound 替代 std::round
        double rounded = qRound(val * 1000) / 1000.0;
        arr.append(rounded);
    }
    return arr;
}
template <int SIZE>
QJsonObject toJson(const IP_sDataPacket<SIZE>& packet) {
    QJsonObject root;

    // 基础信息
    root["addr"] = packet.addr;
    root["status"] = packet.status;
    root["version"] = packet.version;
    root["dev_ip"] = packet.dev_ip;
    root["dev_key"] = packet.dev_key;
    root["datetime"] = packet.datetime;


    // 环境数据（全部转为数组）
    QJsonObject env;
    env["insert"] = QJsonArray::fromVariantList(QVariantList(packet.env_item.insert.begin(), packet.env_item.insert.end()));
    QVariantList list;
    for (double v : packet.env_item.hum_value) {
        list.append(qRound(v));  // 四舍五入
    }
    env["hum_value"] = QJsonArray::fromVariantList(list); //湿度不保留小数

    list.clear();
    for (double v : packet.env_item.tem_value) {
        list.append(qRound(v * 10) / 10.0);  // 四舍五入 + 保留 1 位
    }
    env["tem_value"] = QJsonArray::fromVariantList(list); //温度保留一位

    env["dew_point"] = QJsonArray::fromVariantList(QVariantList(packet.env_item.dew_point.begin(), packet.env_item.dew_point.end()));
    env["hum_alarm_status"] = QJsonArray::fromVariantList(QVariantList(packet.env_item.hum_alarm_status.begin(), packet.env_item.hum_alarm_status.end()));
    env["tem_alarm_status"] = QJsonArray::fromVariantList(QVariantList(packet.env_item.tem_alarm_status.begin(), packet.env_item.tem_alarm_status.end()));




    // 线路数据（自动适配单相/三相）
    QJsonObject line;
    line["cur_alarm_status"] = QJsonArray::fromVariantList(QVariantList(packet.line_item.cur_alarm_status.begin(), packet.line_item.cur_alarm_status.end()));
    line["vol_alarm_status"] = QJsonArray::fromVariantList(QVariantList(packet.line_item.vol_alarm_status.begin(), packet.line_item.vol_alarm_status.end()));
    line["pow_alarm_status"] = QJsonArray::fromVariantList(QVariantList(packet.line_item.pow_alarm_status.begin(), packet.line_item.pow_alarm_status.end()));
    list.clear();
    for (double v : packet.line_item.vol_value) {
        list.append(qRound(v * 10) / 10.0);
    }
    line["vol_value"] = QJsonArray::fromVariantList(list);

    list.clear();
    for (double v : packet.line_item.cur_value) {
        list.append(qRound(v * 10) / 10.0);
    }
    line["cur_value"] = QJsonArray::fromVariantList(list);

    line["cur_alarm_max"] = QJsonArray::fromVariantList(QVariantList(packet.line_item.cur_alarm_max.begin(), packet.line_item.cur_alarm_max.end()));
    line["vol_alarm_max"] = QJsonArray::fromVariantList(QVariantList(packet.line_item.vol_alarm_max.begin(), packet.line_item.vol_alarm_max.end()));

    line["pow_apparent"] = formatToThreeDecimals(packet.line_item.pow_apparent);
    line["pow_reactive"] = formatToThreeDecimals(packet.line_item.pow_reactive);
    line["pow_value"] = formatToThreeDecimals(packet.line_item.pow_value);

    line["power_factor"] = formatToThreeDecimals(packet.line_item.power_factor);

    line["ele_active"] = [&] {
        QVariantList list;
        std::transform(
            packet.line_item.ele_active.begin(),
            packet.line_item.ele_active.end(),
            std::back_inserter(list),
            [](double v) { return QVariant(static_cast<int>(v)); }
            );
        return QJsonArray::fromVariantList(list);
    }();




    // 汇总数据（保持非数组）
    QJsonObject total;
    total["cur_rms"] = packet.total_item.cur_rms;
    total["vol_rms"] = packet.total_item.vol_rms;
    total["ele_active"] = qRound(packet.total_item.ele_active*1)/1;

    total["power_factor"] = qRound(packet.total_item.power_factor * 100) / 100.0;

    total["pow_active"] = qRound(packet.total_item.pow_active*1000)/1000.0;
    total["pow_apparent"] = qRound(packet.total_item.pow_apparent*1000)/1000.0;
    total["pow_reactive"] = qRound(packet.total_item.pow_reactive*1000)/1000.0;
    if(SIZE == 3){

        total["vol_unbalance"] = qRound(packet.total_item.vol_unbalance*10)/10.0;
        total["cur_unbalance"] = qRound(packet.total_item.cur_unbalance*10)/10.0;

    }
    QJsonObject pdu_data;
    pdu_data["env_item_list"] = env;
    pdu_data["line_item_list"] = line;
    pdu_data["pdu_total_data"] = total;

    root["pdu_data"] = pdu_data;

    return root;
}

