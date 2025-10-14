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


void NewTemDlg::on_saveBtn_clicked()
{
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
    auto db = DbTem::build();
    if(!db->insertItem(item)) {
        MsgBox::warning(this, tr("新增模板失败!"));
        return;
    }

    //emit refreshSig(); // 通知刷新表格
    this->close();     // 关闭对话框
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

}


void NewTemDlg::on_Polarity_currentIndexChanged(int index)
{

}


void NewTemDlg::on_FiberSpec_currentIndexChanged(int index)
{

}


void NewTemDlg::on_EditPN_editingFinished()
{

    desc = "SLIM CASSETTE16F LC MTP ULTRA OS2 UNIVERSAL";

}

