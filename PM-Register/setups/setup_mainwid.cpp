#include "setup_mainwid.h"
#include "ui_setup_mainwid.h"

Setup_MainWid::Setup_MainWid(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Setup_MainWid)
{
    ui->setupUi(this);

    groupBox_background_icon(this);
    mUserWid = new UserMainWid(ui->stackedWid);
    ui->stackedWid->addWidget(mUserWid);

    initLogCount();
}

void Setup_MainWid::writeLogCount()
{
    Cfg *cfg = Cfg::bulid();
    if (!cfg || !cfg->item) return;

    int arg1 = ui->logCountSpin->value();
    cfg->item->logCount = arg1;
    cfg->write("log_count", arg1, "Sys");
}

void Setup_MainWid::writePcNum()
{
    Cfg *cfg = Cfg::bulid();
    if (!cfg || !cfg->item) return;

    int arg1 = ui->pcNumSpin->value();
    cfg->item->pcNum = arg1;
    cfg->write("pc_num", arg1, "Sys");
}

void Setup_MainWid::initPcNum()
{
    Cfg *cfg = Cfg::bulid();
    if (!cfg) return;

    int value = cfg->read("pc_num", 0, "Sys").toInt();
    if (cfg->item) {
        cfg->item->pcNum = value;
    }
    ui->pcNumSpin->setValue(value);
}

void Setup_MainWid::initLogCount()
{
    Cfg *cfg = Cfg::bulid();
    if (!cfg) return;

    int value = cfg->read("log_count", 10, "Sys").toInt();
    if (cfg->item) {
        cfg->item->logCount = value * 10000;
    }
    ui->logCountSpin->setValue(value);
}

Setup_MainWid::~Setup_MainWid()
{
    delete ui;
}

void Setup_MainWid::on_pcBtn_clicked()
{
    static int flg = 0;
    QString str = tr("修改");

    bool ret = usr_land_jur();
    if(!ret) {
        MsgBox::critical(this, tr("你无权进行此操作"));
        return;
    }

    if(flg++ %2) {
        ret = false;
        writePcNum();
        writeLogCount();
        Cfg::bulid()->writeCnt();
    } else {
        str = tr("保存");
    }

    ui->pcBtn->setText(str);
    ui->logCountSpin->setEnabled(ret);

    Cfg *cfg = Cfg::bulid();
    if(cfg && cfg->item && cfg->item->pcNum) ret = false;
    ui->pcNumSpin->setEnabled(ret);
}
