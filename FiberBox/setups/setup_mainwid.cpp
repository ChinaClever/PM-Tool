#include "setup_mainwid.h"
#include "ui_setup_mainwid.h"
#include "config.h"
Setup_MainWid::Setup_MainWid(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Setup_MainWid)
{
    ui->setupUi(this);

    groupBox_background_icon(this);
    mUserWid = new UserMainWid(ui->stackedWid);
    ui->stackedWid->addWidget(mUserWid);

    initLogCount();
    initPcNum();

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
    static bool isEditing = false;   // 当前是否处于编辑模式
    QString btnText = tr("修改");

    // 权限检查
    if (!usr_land_jur()) {
        MsgBox::critical(this, tr("你无权进行此操作"));
        return;
    }

    if (isEditing) {
        writePcNum();        // 保存 PC 编号
        writeLogCount();     // 保存 日志计数

        btnText = tr("修改");
        isEditing = false;
    } else {
        btnText = tr("保存");
        isEditing = true;
    }

    ui->pcBtn->setText(btnText);

    ui->logCountSpin->setEnabled(isEditing);
    ui->pcNumSpin->setEnabled(isEditing);
}


void Setup_MainWid::on_Translation_clicked()
{
    qApp->removeTranslator(&translator);

    if(!isEnglish) {
        QString qmPath = QApplication::applicationDirPath() + "/translations/FiberBox_en.qm";
        qDebug()<<qmPath;
        if(translator.load(qmPath)) {
            qApp->installTranslator(&translator);
            isEnglish = true;
            Cfg::bulid()->setLanguage("en");
        }
    } else {
        isEnglish = false;
        Cfg::bulid()->setLanguage("zh");
    }

    ui->retranslateUi(this);

    // 刷新所有控件
    for(QWidget *w : findChildren<QWidget*>()) {
        w->update();
        w->repaint();
    }

    ui->Translation->setText(isEnglish ? tr("Switch to Chinese") : tr("Switch to English"));
}
