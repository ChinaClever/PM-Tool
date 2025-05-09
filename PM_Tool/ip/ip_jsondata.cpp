#include "ip_jsondata.h"
#include "ui_ip_jsondata.h"
#include "backcolourcom.h"
#include "specrannumggen.h"
#include "data_cal/data_cal.h"

#include <QButtonGroup>
#include <QDebug>
#include <math.h>

IP_JsonData::IP_JsonData(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::IP_JsonData)
{
    ui->setupUi(this);
    set_background_icon(this,":/image/box_back.jpg");
    intiSec();
    connect(
        BtnGroup_phase,
        &QButtonGroup::idClicked,
        this,
        &IP_JsonData::ST_Switch_Com_Changed
        );//单三相电压切换
    connect(
        BtnGroup_ele,
        &QButtonGroup::idClicked,
        this,
        &IP_JsonData::set_current
        ); //电流切换
    connect(this, &IP_JsonData::phaseChangeSig, this, &IP_JsonData::set_abled);

    m_timer = new QTimer(this);
    m_timer->setInterval(1000); // 1 秒（单位：毫秒）
    m_timer->setSingleShot(false); // 单次触发（若需循环触发则设为 false）
    // 连接定时器的 timeout 信号到槽函数
    connect(m_timer, &QTimer::timeout, this, &IP_JsonData::powerdataCal);
    m_timer->start(); // 启动定时器

    incr_timer = new QTimer(this);
    incr_timer->setInterval(1000);
    incr_timer->setSingleShot(false);
    connect(incr_timer, &QTimer::timeout, this, &IP_JsonData::incrCal);
    incr_timer->start(); // 启动定时器

}

IP_JsonData::~IP_JsonData()
{
    delete ui;
}

void IP_JsonData::intiSec() //IP启动初始化
{
    BtnGroup_phase = new QButtonGroup;
    BtnGroup_ele = new QButtonGroup;
    BtnGroup_phase->addButton(ui->tpe,0);  //三相
    BtnGroup_phase->addButton(ui->spe,1);   //单相
    ui->tpe->setChecked(true);

    BtnGroup_ele->addButton(ui->cur32A,0);
    BtnGroup_ele->addButton(ui->cur63A,1);
    BtnGroup_ele->addButton(ui->cur125A,2);
    ui->cur63A->setChecked(true);

    ui->volA->setValue(specRanNumGgen::generate_voltage_double());
    ui->volB->setValue(specRanNumGgen::generate_voltage_double());
    ui->volC->setValue(specRanNumGgen::generate_voltage_double());

    ui->curA->setValue(specRanNumGgen::generate_phase_current(63));
    ui->curB->setValue(specRanNumGgen::generate_phase_current(63));
    ui->curC->setValue(specRanNumGgen::generate_phase_current(63));


    ui->pfA->setValue(specRanNumGgen::get_power_factor_precise());
    ui->pfB->setValue(specRanNumGgen::get_power_factor_precise());
    ui->pfC->setValue(specRanNumGgen::get_power_factor_precise());

    ui->envTem->setValue(specRanNumGgen::get_temperature());
    ui->envHum->setValue(specRanNumGgen::get_humidity());
}

void IP_JsonData::ST_Switch_Com_Changed(int index)
{
    /*
     0：三相  显示BC相控件
     1：单相  隐藏BC相控件
     */
    if(index == 0){
        emit phaseChangeSig(1);
    }else{
        emit phaseChangeSig(0);
    }
}
void IP_JsonData::set_current(int index)
{
    double cur  = (index == 0) ? 32 : (index == 1) ? 63 : 125;

    ui->curCapA->setValue(cur);
    ui->curCapB->setValue(cur);
    ui->curCapC->setValue(cur);


}

void IP_JsonData::set_abled(int id)
{
    ui->volB->setEnabled(id);
    ui->volC->setEnabled(id);
    ui->curB->setEnabled(id);
    ui->curC->setEnabled(id);
    ui->pfB->setEnabled(id);
    ui->pfC->setEnabled(id);
    ui->eleB->setEnabled(id);
    ui->eleC->setEnabled(id);
}
void IP_JsonData::powerdataCal()
{
    int phase = BtnGroup_phase->checkedId();
    //qDebug()<<phase;

    //ui->act_PowA->setValue();
    ui->ApparentA->setValue(data_cal::apparent_powerCal(ui->volA->value(),ui->curA->value()));
    ui->act_PowA->setValue(data_cal::active_powerCal(ui->volA->value(),ui->curA->value(),ui->pfA->value()));
    ui->Rec_PowA->setValue(data_cal::reactive_powerCal(ui->ApparentA->value(),ui->act_PowA->value()));
    if(!phase){

        ui->ApparentB->setValue(data_cal::apparent_powerCal(ui->volB->value(),ui->curB->value()));
        ui->ApparentC->setValue(data_cal::apparent_powerCal(ui->volC->value(),ui->curC->value()));

        ui->act_PowB->setValue(data_cal::active_powerCal(ui->volA->value(),ui->curB->value(),ui->pfB->value()));
        ui->act_PowC->setValue(data_cal::active_powerCal(ui->volA->value(),ui->curC->value(),ui->pfC->value()));

        ui->Rec_PowB->setValue(data_cal::reactive_powerCal(ui->ApparentB->value(),ui->act_PowB->value()));
        ui->Rec_PowC->setValue(data_cal::reactive_powerCal(ui->ApparentC->value(),ui->act_PowC->value()));

    }
}

void IP_JsonData::incrCal()
{
    if (double y = ui->envInc->value()) {
        double x = ui->envTem->value();
        double newVal = incrEnvInc ? x + y : x - y;
        ui->envTem->setValue(incrEnvInc ? fmin(x + y, 100) : fmax(x - y, 0));
        incrEnvInc = (incrEnvInc && newVal >= 100) ? false :
                         (!incrEnvInc && newVal <= 0)   ? true  :
                         incrEnvInc;
    }

    if(int y = ui->envhumInc->value()){
        int x = ui->envHum->value();
        int newVal = incrEnvHum ? x + y : x - y;
        ui->envHum->setValue(incrEnvHum ? fmin(x + y, 100) : fmax(x - y, 0));
        incrEnvHum = (incrEnvHum && newVal >= 100) ? false :
                         (!incrEnvHum && newVal <= 0)   ? true  :
                         incrEnvHum;
    }
}
