#include "ip_bulksend.h"
#include "ui_ip_bulksend.h"
#include <QButtonGroup>
#include "data_cal/data_cal.h"
#include "globals.h"
#include "specrannumggen.h"

ip_BulkSend::ip_BulkSend(QWidget *parent)
    : QWidget(parent)
    ,m_tmapProcessor(new TMapProcessor(this))
    ,m_triphasejson(new TriPhaseJsonQueue(this))
    , ui(new Ui::ip_BulkSend)
{
    ui->setupUi(this);

    numChangeconnect();

    inti();
    connect(this,&ip_BulkSend::tmpchange,m_tmapProcessor,&TMapProcessor::changerun);
}
void ip_BulkSend::numChangeconnect()
{
    QList<QSpinBox*> spinBoxes = {
        ui->tpeNum,
        ui->speNum,
        ui->Taddnum,
        ui->Saddnum,
        ui->Ttimeinv,
        ui->Stimeinv
    };

    for (QSpinBox* spinBox : spinBoxes) {
        connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                this, &ip_BulkSend::STNumchange);
    }
}
void ip_BulkSend::inti()
{

    devip = ui->devIp->text();
    ui->devIp->setValidator(new QRegExpValidator(QRegExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b")));
    Sdevip = ui->SdevIp->text();
    ui->SdevIp->setValidator(new QRegExpValidator(QRegExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b")));
    tpe = ui->tpeNum->value();
    spe = ui->speNum->value();
    Tnum = ui->Taddnum->value();
    Snum = ui->Saddnum->value();
    Ttime = ui->Ttimeinv->value();
    Stime = ui->Stimeinv->value();

}

ip_BulkSend::~ip_BulkSend()
{
    delete ui;
}

void ip_BulkSend::on_bulkSendBtn_clicked()
{

    if(ui->bulkSendBtn->text()=="开始发送"){
        ui->bulkSendBtn->setText("停止发送");

        intiMap(3);
        emit tmpchange(1);

        m_tmapProcessor->start();
        //m_triphasejson->start();
    }
    else{
        ui->bulkSendBtn->setText("开始发送");

        emit tmpchange(0);

    }
}
void ip_BulkSend::on_SbulkSendBtn_clicked()
{
    if(ui->SbulkSendBtn->text()=="开始发送"){
        ui->SbulkSendBtn->setText("停止发送");

        intiMap(1);


    }
    else{
        ui->SbulkSendBtn->setText("开始发送");
    }
}
void ip_BulkSend::bulkinti(const int x)
{


    if(x == 3){
        QString key = data_cal::generateNextCascadeIP(devip,addr,Tnum);
        auto packet = IP_sDataPacket<3>::create();  // SIZE=3

        packet.setBasicParams(addr, devip, key);
        packet.datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

        packet.intiline(ui->TcurCap->value());
        packet.env_item.tem_value[0] = specRanNumGgen::get_temperature();
        packet.env_item.hum_value[0] = specRanNumGgen::get_humidity();
        packet.env_item.dew_point[0] = data_cal::calculate_dewpoint1(packet.env_item.tem_value[0], packet.env_item.hum_value[0]);

        packet.totalDataCal();
        packet.Timesend = Ttime;
        tMap[key] = packet;
      //  qDebug() << "Packet info:\n" << packet;
    }
    else{
        QString Skey = data_cal::generateNextCascadeIP(Sdevip,Saddr,Snum);
        auto packet = IP_sDataPacket<1>::create();  // SIZE=3

        packet.setBasicParams(Saddr, Sdevip, Skey);
        packet.datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

        packet.intiline(ui->ScurCap->value());
        packet.env_item.tem_value[0] = specRanNumGgen::get_temperature();
        packet.env_item.hum_value[0] = specRanNumGgen::get_humidity();
        packet.totalDataCal();
        packet.Timesend = Stime;
        sMap[Skey] = packet;
        qDebug() << "Packet info:\n" << packet;
    }
}
void ip_BulkSend::intiMap(const int x)
{
    qDebug() <<tpe;
    if(x==3){
        for(int i=0;i<tpe*Tnum;i++){
            bulkinti(3);
        }
    }
    else {
        qDebug()<<Sdevip;
        for(int i=0;i<spe*Snum;i++){
            bulkinti(1);
        }
    }

}
void ip_BulkSend::STNumchange()
{
    qDebug()<<"===========";

    tpe = ui->tpeNum->value();
    Tnum = ui->Taddnum->value();
    spe = ui->speNum->value();
    Snum = ui->Saddnum->value();
    Ttime = ui->Ttimeinv->value();
    Stime = ui->Stimeinv->value();
}


