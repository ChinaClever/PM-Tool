#include "send_json.h"
#include "ui_send_json.h"
#include "json_data.h"
#include <QDebug>
#include <QJsonArray>
#include <QDateTime>
#include <QtMath>
#include <algorithm>
#include <QUdpSocket>
#include <QHostAddress>
#include <QJsonDocument>


send_json::send_json(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::send_json)
{
    ui->setupUi(this);
    inti();
}

send_json::~send_json()
{
    delete ui;
}

void send_json::inti()
{
    ui->Addr_Edit->setValidator(new QIntValidator(ui->Addr_Edit));
    ui->dev_Ip->setValidator(new QRegExpValidator(QRegExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b")));
    //portInput->setValidator(new QIntValidator(0, 65535, portInput));
    ui->Port->setValidator(new QIntValidator(0, 65532,ui->Port));

    ui->Status_Box->setCurrentIndex(0);
    ui->envHum1->setEnabled(0);
    ui->envTem1->setEnabled(0);
    ui->envTemAlarm1->setEnabled(0);
    ui->envHumAlarm1->setEnabled(0);
    ui->envHum2->setEnabled(0);
    ui->envTem2->setEnabled(0);
    ui->envTemAlarm2->setEnabled(0);
    ui->envHumAlarm2->setEnabled(0);
}

void send_json::on_ST_Switch_Com_currentIndexChanged(int index)
{
    ui->ST_Switch->setCurrentIndex(index);
    qDebug()<<ui->ST_Switch->currentIndex();
}


void send_json::on_Submit_Json_Btn_clicked()
{
    int addr = ui->Addr_Edit->text().toInt();
    int status = ui->Status_Box->currentIndex();
    int version = ui->Version_Box->value();
    QString devIp = ui->dev_Ip->text().trimmed();
    QString dev_key = devIp + "-" + QString("%1").arg(addr, 2, 10, QChar('0'));
    QString datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString uuid = ui->uuid->text().trimmed();
    QString pdu_alarm = "";

    QJsonObject env_item_list;
    QJsonObject lineItemList;
    QJsonObject pdu_total_data;

    QJsonObject pduData;

    QVector<double> vol_value;
    QVector<int> vol_alarm_status;

    //单三相电赋值
    {
        int phaseCount = ui->ST_Switch->currentIndex();
        if(!phaseCount)phaseCount = 3;

        vol_value = {ui->lineVolA_3->value(), ui->lineVolB_3->value(), ui->lineVolC_3->value()};
        vol_alarm_status = {ui->lineVolAlarmA_3->currentIndex(), ui->lineVolAlarmB_3->currentIndex(), ui->lineVolAlarmC_3->currentIndex()};
        //三相电
        auto x = ui->lineVolA_3->value();
        qDebug()<<x;
        Json_Data jsonData;  // 先实例化
        if(phaseCount == 3){
            lineItemList = jsonData.generateLineItemList(
                phaseCount,
                {ui->lineCurAlarmA_3->currentIndex(), ui->lineCurAlarmB_3->currentIndex(), ui->lineCurAlarmC_3->currentIndex()},
                {ui->lineCurMaxA_3->value(), ui->lineCurMaxB_3->value(), ui->lineCurMaxC_3->value()},
                {ui->lineCurA_3->value(),ui->lineCurB_3->value(), ui->lineCurC_3->value()},
                {ui->lineEleA_3->value(), ui->lineEleB_3->value(), ui->lineEleC_3->value()},
                {ui->linePfA_3->value(), ui->linePfB_3->value(), ui->linePfC_3->value()},
                {ui->linePowA_3->value(), ui->linePowB_3->value(), ui->linePowC_3->value()},
                {ui->lineApparentA_3->value(), ui->lineApparentB_3->value(), ui->lineApparentC_3->value()},
                {0,0,0},
                {ui->lineVolAlarmA_3->currentIndex(), ui->lineVolAlarmB_3->currentIndex(), ui->lineVolAlarmC_3->currentIndex()},
                {ui->lineVolA_3->value(), ui->lineVolB_3->value(), ui->lineVolC_3->value()}
                );
        }
        else{
            vol_value = {ui->lineVolA_1->value()};
            vol_alarm_status = {ui->lineVolAlarmA_1->currentIndex()};

            lineItemList = jsonData.generateLineItemList(
                phaseCount,
                {ui->lineCurAlarmA_1->currentIndex()},
                {ui->lineCurMaxA_1->value()},
                {ui->lineCurA_1->value()},
                {ui->lineEleA_1->value()},
                {ui->linePfA_1->value()},
                {ui->linePowA_1->value()},
                {ui->lineApparentA_1->value()},
                {0},
                {ui->lineVolAlarmA_1->currentIndex()},
                {ui->lineVolA_1->value()}
                );
        }
        QString Alarm = jsonData.generateVoltageAlarmDescription(vol_value,vol_alarm_status);
        qDebug()<<Alarm;
        pdu_alarm = Alarm;
    }

    //传感器
    {
        Json_Data jsonData;

        QList<int> insert = {ui->envInsert1->currentIndex(), ui->envInsert2->currentIndex()}; // 第1个插入，第2个未插
        QList<double> hum = {ui->envHum1->value(), ui->envHum2->value()};
        QList<double> tem = {ui->envTem1->value(), ui->envTem2->value()};
        QList<int> temAlarm = {turn(ui->envTemAlarm1->currentIndex()), turn(ui->envTemAlarm2->currentIndex())};
        QList<int> humAlarm = {turn(ui->envHumAlarm1->currentIndex()), turn(ui->envHumAlarm1->currentIndex())};

        env_item_list = jsonData.generateEnvItemList(insert, hum, tem, humAlarm, temAlarm);
        // if (!env_item_list.isEmpty()) {
        //     jsonData.setEnvItemList(env_item_list);
        // }
        qDebug()<<env_item_list;
    }

    //totalData
    {
        Json_Data jsonData;

        double cur_rms = 0.0;
        double vol_rms;
        double ele_active;
        double power_factor;
        double pow_active;
        double pow_apparent;
        double pow_reactive;
        double vol_unbalance;
        double cur_unbalance;
        int phaseCount = ui->ST_Switch->currentIndex();
        if(!phaseCount)phaseCount = 3;
        if(phaseCount == 1){
            cur_rms = ui->lineCurA_1->value();
            vol_rms = ui->lineVolA_1->value();
            ele_active = ui->lineEleA_1->value();
            pow_active = ui->linePowA_1->value();
            pow_apparent = ui->lineApparentA_1->value();
            power_factor = pow_active/pow_apparent*1.0;
            pow_reactive = 0; //预留
            vol_unbalance = 0;
            cur_unbalance = 0;
        } else{
            //计算方法是先平方、再平均，然后再开方
            cur_rms = sqrt((ui->lineCurA_3->value()*ui->lineCurA_3->value()
                            + ui->lineCurB_3->value()*ui->lineCurB_3->value()
                            + ui->lineCurC_3->value()*ui->lineCurC_3->value())
                           /3.0
                           );
            vol_rms = sqrt((ui->lineVolA_3->value()*ui->lineVolA_3->value()
                            + ui->lineVolB_3->value()*ui->lineVolB_3->value()
                            + ui->lineVolC_3->value()*ui->lineVolC_3->value())
                           /3.0
                           );

            ele_active = ui->lineEleA_3->value() + ui->lineEleB_3->value() + ui->lineEleC_3->value();

            pow_active = ui->linePowA_3->value() + ui->linePowB_3->value() + ui->linePowC_3->value();
            pow_apparent = ui->lineApparentA_3->value() + ui->lineApparentB_3->value() + ui->lineApparentC_3->value();

            power_factor = pow_active/pow_apparent*1.0;

            pow_reactive = 0;//预留
//三相不平衡度            （最大相电压-最小相电压）/平均相电压×100%

            if((ui->lineVolA_3->value()
                 + ui->lineVolB_3->value()
                 + ui->lineVolC_3->value()
                 )==0){
                vol_unbalance = 0;
            }else{
                vol_unbalance = (std::max({ui->lineVolA_3->value()
                                       , ui->lineVolB_3->value()
                                       , ui->lineVolC_3->value()})
                                 - std::min({ui->lineVolA_3->value()
                                             , ui->lineVolB_3->value()
                                             , ui->lineVolC_3->value()})
                                 )
                                / ((ui->lineVolA_3->value()
                                    + ui->lineVolB_3->value()
                                    + ui->lineVolC_3->value()
                                    )/3.0
                                   )*100;
            }



            if((ui->lineCurA_3->value()
                 + ui->lineCurB_3->value()
                 + ui->lineCurC_3->value()
                 )==0){
                cur_unbalance = 0;
            }
            else{
                cur_unbalance = (std::max({ui->lineCurA_3->value()
                                        , ui->lineCurB_3->value()
                                        , ui->lineCurC_3->value()})
                                 - std::min({ui->lineCurA_3->value()
                                             , ui->lineCurB_3->value()
                                             , ui->lineCurC_3->value()})
                                 )
                                / ((ui->lineCurA_3->value()
                                    + ui->lineCurB_3->value()
                                    + ui->lineCurC_3->value()
                                    )/3.0
                                   )*100
                    ;
            }
        }


        pdu_total_data = jsonData.generatePduTotalData(
                cur_rms,
                vol_rms,
                ele_active,
                power_factor,
                pow_active,
                pow_apparent,
                pow_reactive,
                vol_unbalance,
                cur_unbalance
            );
    }

    //pdu_data
    {
        Json_Data jsonData;
        pduData = jsonData.generatePduData(env_item_list,lineItemList,pdu_total_data);

    }


    Json_Data jsonData;

    jsonData.setBasicInfo(
        addr, status, version,
        devIp, dev_key, datetime,
        pdu_alarm, env_item_list,
        lineItemList, pdu_total_data,
        pduData, uuid
        );

    QJsonObject root = jsonData.generateJson();
     root = jsonData.getJsonObject(); //最终发送的json对象
    qDebug()<<"================";
    qDebug()<<root;
    sendJsonViaUdp(root);

}




void send_json::on_envInsert1_currentIndexChanged(int index)  //限制env未插入时其他框不可填写
{
    bool inserted = (index == 1);  // 1 是“插入”，0 是“未插入”

    ui->envHum1->setEnabled(inserted);
    ui->envTem1->setEnabled(inserted);
    ui->envTemAlarm1->setEnabled(inserted);
    ui->envHumAlarm1->setEnabled(inserted);
    if (!inserted) {
        // 未插入时清零
        ui->envHum1->setValue(0);
        ui->envTem1->setValue(0);
        ui->envTemAlarm1->setCurrentIndex(0);
        ui->envHumAlarm1->setCurrentIndex(0);
    }
}
void send_json::on_envInsert2_currentIndexChanged(int index)
{
    bool inserted = (index == 1);  // 1 是“插入”，0 是“未插入”

    ui->envHum2->setEnabled(inserted);
    ui->envTem2->setEnabled(inserted);
    ui->envTemAlarm2->setEnabled(inserted);
    ui->envHumAlarm2->setEnabled(inserted);

    if (!inserted) {
        // 未插入时清零
        ui->envHum2->setValue(0);
        ui->envTem2->setValue(0);
        ui->envTemAlarm2->setCurrentIndex(0);
        ui->envHumAlarm2->setCurrentIndex(0);
    }
}



int send_json::turn(int x)
{
    if(x==0||x==1||x==2)return x;
    else if(x==3)return 4;
    else return 8;
}


void send_json::on_Reset_Btn_clicked()   //重置按钮，部分信息重置未写
{
    ui->envInsert1->setCurrentIndex(0);
    ui->envInsert2->setCurrentIndex(0);

    ui->Addr_Edit->setValidator(new QIntValidator(ui->Addr_Edit));
    ui->dev_Ip->setValidator(new QRegExpValidator(QRegExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b")));
    ui->envHum1->setEnabled(0);
    ui->envTem1->setEnabled(0);
    ui->envTemAlarm1->setEnabled(0);
    ui->envHumAlarm1->setEnabled(0);
    ui->envHum2->setEnabled(0);
    ui->envTem2->setEnabled(0);
    ui->envTemAlarm2->setEnabled(0);
    ui->envHumAlarm2->setEnabled(0);
}

void send_json::sendJsonViaUdp(const QJsonObject &json)     //发送json数据包，与对应py脚本运行
{
    QUdpSocket *udpSocket = new QUdpSocket(this);

    qDebug() << "发送前内容: " << QJsonDocument(json).toJson(QJsonDocument::Indented);

    QByteArray jsonData = QJsonDocument(json).toJson(QJsonDocument::Compact);

    QString targetIp = "127.0.0.1";
    quint16 targetPort = 12345;


    udpSocket->writeDatagram(jsonData, QHostAddress(targetIp), targetPort);

    udpSocket->deleteLater(); // 发送完释放资源
}
