#include "ip_bulksend.h"
#include "ui_ip_bulksend.h"
#include <QButtonGroup>
#include "data_cal/data_cal.h"
#include "globals.h"
#include "specrannumggen.h"
#include <QMessageBox>
#include "stylehelper.h"


#define SriNum 5 //单三相线程宏

ip_BulkSend::ip_BulkSend(QWidget *parent)
    : QWidget(parent)
    ,m_tmapProcessor(new TMapProcessor(this))
    ,m_smapProcessor(new SMapProcessor(this))
    , ui(new Ui::ip_BulkSend)
{
    ui->setupUi(this);
    StyleHelper::setLightBlueButton(ui->SbulkSendBtn);
    StyleHelper::setLightBlueButton(ui->bulkSendBtn);



    for(int i=0;i<SriNum;i++){
        m_sriphasejson.push_back(new SriPhaseJsonQueue(this));
        m_triphasejson.push_back(new TriPhaseJsonQueue(this));
    }

    inti();
    numChangeconnect();
    connect(this,&ip_BulkSend::tmpchange,m_tmapProcessor,&TMapProcessor::Tchangerun);
    connect(this,&ip_BulkSend::smpchange,m_smapProcessor,&SMapProcessor::Schangerun);

    for(int i=0;i<SriNum;i++){
        connect(this,&ip_BulkSend::smpchange,m_sriphasejson[i],&SriPhaseJsonQueue::sriRun);
        connect(this,&ip_BulkSend::smpchange,m_triphasejson[i],&TriPhaseJsonQueue::triRun);
    }

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

    Stimesend = ui->Stimeinv->value();
    Ttimesend = ui->Ttimeinv->value();
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

void ip_BulkSend::on_bulkSendBtn_clicked()  //三相发送启动
{

    if(ui->bulkSendBtn->text()=="开始发送"){

        ui->bulkSendBtn->setText("停止发送");

        ui->TcurCap->setEnabled(0);
        ui->devIp->setEnabled(0);
        ui->tpeNum->setEnabled(0);
        ui->Taddnum->setEnabled(0);
        ui->Ttimeinv->setEnabled(0);

        intiMap(3);

        emit tmpchange(1);
        m_tmapProcessor->start(); //线程启动
        for(int i=0;i<SriNum;i++){
            m_triphasejson[i]->start();
        }
    }
    else{
        ui->bulkSendBtn->setText("开始发送");

        ui->TcurCap->setEnabled(1);
        ui->devIp->setEnabled(1);
        ui->tpeNum->setEnabled(1);
        ui->Taddnum->setEnabled(1);
        ui->Ttimeinv->setEnabled(1);

        emit tmpchange(0);  //关闭线程
        tMap.clear();
        devip = ui->devIp->text();
        addr = -1;

    }
}

void ip_BulkSend::on_SbulkSendBtn_clicked() //单相发送启动
{
    if(ui->SbulkSendBtn->text()=="开始发送"){


        ui->SbulkSendBtn->setText("停止发送");

        ui->ScurCap->setEnabled(0);
        ui->SdevIp->setEnabled(0);
        ui->speNum->setEnabled(0);
        ui->Saddnum->setEnabled(0);
        ui->Stimeinv->setEnabled(0);

        intiMap(1);
        emit smpchange(1); //改变线程变量

        m_smapProcessor->start();

        for(int i=0;i<SriNum;i++){
            m_sriphasejson[i]->start();
        }

    }
    else{
        ui->SbulkSendBtn->setText("开始发送");
        ui->ScurCap->setEnabled(1);
        ui->SdevIp->setEnabled(1);
        ui->speNum->setEnabled(1);
        ui->Saddnum->setEnabled(1);
        ui->Stimeinv->setEnabled(1);

        emit smpchange(0);
        Sdevip = ui->SdevIp->text();
        Saddr = -1;

    }
}

void ip_BulkSend::triggerToggleSend(bool flag)
{
    if(flag) {
        if(ui->bulkSendBtn->text() == "开始发送") {
            on_bulkSendBtn_clicked();
        }
        if(ui->SbulkSendBtn->text() == "开始发送") {
            on_SbulkSendBtn_clicked();
        }
    } else {
        if(ui->bulkSendBtn->text() == "停止发送") {
            on_bulkSendBtn_clicked();
        }
        if(ui->SbulkSendBtn->text() == "停止发送") {
            on_SbulkSendBtn_clicked();
        }
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

       // qDebug() << "Packet info:\n" << key;
    }
    else{
        QString Skey = data_cal::generateNextCascadeIP(Sdevip,Saddr,Snum);
        auto packet = IP_sDataPacket<1>::create();  // SIZE=1

        packet.setBasicParams(Saddr, Sdevip, Skey);
        packet.datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

        packet.intiline(ui->ScurCap->value());
        packet.env_item.tem_value[0] = specRanNumGgen::get_temperature();
        packet.env_item.hum_value[0] = specRanNumGgen::get_humidity();
        packet.env_item.dew_point[0] = data_cal::calculate_dewpoint1(packet.env_item.tem_value[0], packet.env_item.hum_value[0]);

        packet.totalDataCal();
        packet.Timesend = Stime;

        sMap[Skey] = packet;
        //qDebug() <<sMap.size();
    }
}

void ip_BulkSend::intiMap(const int x) //判断启动项单三相，确定创建主机个数
{
    //qDebug() <<tpe;
    if(x==3){
        devip = ui->devIp->text();
        addr = -1;
        for(int i=0;i<tpe;i++){
            bulkinti(3);
        }
    }
    else {
        //qDebug()<<Sdevip;
        Sdevip = ui->SdevIp->text();
        Saddr = -1;
        for(int i=0;i<spe;i++){
            bulkinti(1);
        }
    }

}

void ip_BulkSend::STNumchange() //单三相参数变化
{
    qDebug()<<"===========";

    Ttimesend = ui->Ttimeinv->value();
    Stimesend = ui->Stimeinv->value();
    tpe = ui->tpeNum->value();
    Tnum = ui->Taddnum->value();
    spe = ui->speNum->value();
    Snum = ui->Saddnum->value();
    Ttime = ui->Ttimeinv->value();
    Stime = ui->Stimeinv->value();
}

void ip_BulkSend::on_SdevIp_editingFinished()
{
    Sdevip = ui->SdevIp->text();
}

void ip_BulkSend::on_devIp_editingFinished()
{
    devip = ui->devIp->text();
}

void ip_BulkSend::saveSettings(QSettings &settings) {
    settings.beginGroup("Ipbulk");
    SettingsHelper::saveSpinBox(settings, "speNum", ui->speNum);
    SettingsHelper::saveSpinBox(settings, "tpeNum", ui->tpeNum);
    settings.endGroup();
}

void ip_BulkSend::loadSettings(QSettings &settings) {
    settings.beginGroup("Ipbulk");
    SettingsHelper::loadSpinBox(settings, "speNum", ui->speNum);
    SettingsHelper::loadSpinBox(settings, "tpeNum", ui->tpeNum);
    settings.endGroup();
}
