#include <QString>

extern QString serIp;  //批量发送处理目标ip
extern QString port;   //目标 pdu端口
extern QString BusPort; //目标 母线端口


extern QString serIp2;
extern QString port2;
extern QString BusPort2;

extern QString serIp3;
extern QString port3;
extern QString BusPort3;

extern QString mqttserIp;
extern QString mqttport;
extern QString mqttBusPort;

extern bool enableUdp;    // true = UDP
extern bool enableMqtt;   // true = MQTT

extern int mqttKeepAlive;      // 心跳秒数
extern int mqttQos;            // 发送 Qos

enum class SendMode {
    UDP,
    MQTT,
    NONE
};
extern SendMode sendMode;
