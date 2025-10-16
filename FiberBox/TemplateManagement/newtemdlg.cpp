#include "newtemdlg.h"
#include "ui_newtemdlg.h"
#include "backcolourcom.h"
#include "dbtem.h"
#include "msgbox.h"
NewTemDlg::NewTemDlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NewTemDlg)
{
    ui->setupUi(this);
    this->setWindowTitle("新增模板");
    set_background_icon(this,":/image/back.jpg");
}

NewTemDlg::~NewTemDlg()
{
    delete ui;
}

void NewTemDlg::on_cancelBtn_clicked()
{
    this->close();
}

void NewTemDlg::loadTemInfo(sTemItem &tem)
{
    // ===== 基本字段 =====
    ui->EditPN->setText(tem.data.PN);
    ui->EditDesc->setPlainText(tem.data.description);
    ui->FanoutPN->setText(tem.data.FanoutPn);
    ui->FanoutCount->setCurrentText(QString::number(tem.data.FanCount));

    // ===== 模式和波长 =====
    if (tem.data.info.mode == FiberMode::SM) {
        int idx = ui->WaveLambda->findText("SM", Qt::MatchContains);
        if (idx >= 0) ui->WaveLambda->setCurrentIndex(idx);
    }
    else if (tem.data.info.mode == FiberMode::MM) {
        int idx = ui->WaveLambda->findText("MM", Qt::MatchContains);
        if (idx >= 0) ui->WaveLambda->setCurrentIndex(idx);
    }
    else {
        ui->WaveLambda->setCurrentIndex(0);
    }


    // ===== 接口类型与光纤数 =====
    QString ifaceStr = ifaceToString(tem.data.info.iface);   // e.g. "MTP"
    QString countStr = countToString(tem.data.info.count);   // e.g. "16F"

    // 所以尝试组合匹配
    int ifaceIndex = ui->Interface->findText(ifaceStr);
    if (ifaceIndex < 0)
        ifaceIndex = ui->Interface->findText(ifaceStr + "-" + countStr);
    ui->Interface->setCurrentIndex(ifaceIndex >= 0 ? ifaceIndex : 0);

    ui->FiberCount->setCurrentText(countStr);

    // ===== 其他下拉框字段 =====
    ui->Polarity->setCurrentText(polarityToString(tem.data.info.polarity));
    ui->FiberSpec->setCurrentText(specToString(tem.data.info.spec));
    ui->QRTem->setCurrentText(tem.data.qrTemplate);
    ui->LabelTem->setCurrentText(tem.data.labelTemplate);
    ui->ILMax->setValue(tem.data.limit);

    // ===== 记录当前时间（仅展示用）=====
    qDebug() << "加载模板:" << tem.data.PN << "描述:" << tem.data.description;
}


void NewTemDlg::editTitle(const QString& str = "新增模板")
{
    ui->EditPN->setEnabled(false);
    this->setWindowTitle(str);
    //checkUsr();
}

bool NewTemDlg::saveTemInfo(sTemItem& item)
{
    auto db = DbTem::build();
    int rtn = db->getItemByPN(item.data.PN,item);
    if(rtn > 0) return false;

    return db->insertItem(item);
}

bool NewTemDlg::inputCheck()
{
    return true;
}

void NewTemDlg::on_saveBtn_clicked()
{
    if(inputCheck()){
        bool ret = true;
        sTemItem item;
        item.data.PN = ui->EditPN->text();
        item.data.description = ui->EditDesc->toPlainText();
        item.data.FanoutPn = ui->FanoutPN->text();
        item.data.FanCount = ui->FanoutCount->currentText().toInt();

        item.data.info.mode = stringToMode(ui->WaveLambda->currentIndex() == 0 ? "SM" : "MM");
        if(item.data.info.mode == FiberMode::SM){
            item.data.lambda.first = 1310;
            item.data.lambda.second = 1550;
        }
        else if(item.data.info.mode == FiberMode::MM){
            item.data.lambda.first = 850;
            item.data.lambda.second = 1300;
        }
        else{
            ret = false;
        }

        item.data.info.iface = stringToIface(ui->Interface->currentText());
        item.data.info.count = stringToCount(ui->FiberCount->currentText());
        item.data.info.polarity = stringToPolarity(ui->Polarity->currentText());
        item.data.info.spec = stringToSpec(ui->FiberSpec->currentText());
        item.data.qrTemplate = ui->QRTem->currentText();
        item.data.labelTemplate = ui->LabelTem->currentText();
        item.data.limit = ui->ILMax->value();

        item.date = QDate::currentDate().toString("yyyy-MM-dd");
        item.time = QTime::currentTime().toString("HH:mm:ss");

        if(!ret){
            MsgBox::warning(this, tr("模板数据错误!"));
            return ;
        }

        if(saveTemInfo(item))
            accept();
        else
            MsgBox::critical(this,  tr("模板已存在"));
    }
}


void NewTemDlg::on_FiberCount_currentIndexChanged(int index)
{
    ui->QRTem->setCurrentIndex(ui->FiberCount->currentIndex());

    int a = ui->FanoutCount->currentIndex();
    int b = ui->FiberCount->currentIndex();
    int c ;
    if(a == 0 && b == 2)c = 0;
    if(a == 1 && b == 0)c = 1;
    if(a == 0 && b == 1)c = 2;
    if(a == 1 && b == 1)c = 3;
    if(a == 2 && b == 0)c = 4;
    if(a == 1 && b == 2)c = 5;

    ui->LabelTem->setCurrentIndex(c);
    updateDescription();
}


void NewTemDlg::on_FanoutCount_currentIndexChanged(int index)
{
    int a = ui->FanoutCount->currentIndex();
    int b = ui->FiberCount->currentIndex();
    int c ;
    if(a == 0 && b == 2)c = 0;
    if(a == 1 && b == 0)c = 1;
    if(a == 0 && b == 1)c = 2;
    if(a == 1 && b == 1)c = 3;
    if(a == 2 && b == 0)c = 4;
    if(a == 1 && b == 2)c = 5;

    ui->LabelTem->setCurrentIndex(c);
}

void NewTemDlg::on_Interface_currentIndexChanged(int index)
{
    updateDescription();
}


void NewTemDlg::on_Polarity_currentIndexChanged(int index)
{
    //updateDescription();
}


void NewTemDlg::on_FiberSpec_currentIndexChanged(int index)
{
    updateDescription();
}


void NewTemDlg::on_EditPN_editingFinished()
{
    updateDescription();
}

void NewTemDlg::updateDescription()
{
    // 获取界面当前选择的值
    QString iface = ui->Interface->currentText();      // 比如 "MTP-LC"
    QString fiberCount = ui->FiberCount->currentText(); // 比如 "16F"
    QString spec = ui->FiberSpec->currentText();        // 比如 "OS2"

    QString flippedIface = iface;
    if (iface.contains('-')) {
        QStringList parts = iface.split('-');  // ["MTP", "LC"]
        if (parts.size() == 2)
            flippedIface = parts[1] + " " + parts[0];  // "LC MTP"
    }

    // 格式化描述字符串
    QString desc = QString("SLIM CASSETTE %1 %2 ULTRA %3")
                       .arg(fiberCount)
                       .arg(iface)
                       .arg(spec);

    ui->EditDesc->setText(desc);
}
