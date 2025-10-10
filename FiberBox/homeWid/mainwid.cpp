#include "mainwid.h"
#include "ui_mainwid.h"
#include "backcolourcom.h"
#include "fibertab.h"

#include <QDebug>
#include <QMessageBox>
mainWid::mainWid(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::mainWid)
{
    ui->setupUi(this);
    set_background_icon(this,":/image/back.jpg");
    getInstCon();
    exmple();
}

mainWid::~mainWid()
{
    delete ui;
}

void mainWid::exmple()
{

    msgCenter->sendTip("Test",Qt::red);

    ui->tabWidget->clear();


    for (int i = 1; i <= 3; ++i) {
        FiberTab *tab = new FiberTab();
        ui->tabWidget->addTab(tab, QString("扇出线%1").arg(i));
    }

    for (int i = 0; i < ui->tabWidget->count(); ++i) {
        FiberTab *tab = qobject_cast<FiberTab*>(ui->tabWidget->widget(i));
        if (tab) {
            tab->setInfo("A018497AA 0248010005", 12, "1310nm/1550nm", "0.50dB");

            QString rawData = R"(
Fiber 01: 0.22dB / 0.21dB
Fiber 02: 0.15dB / 0.13dB
Fiber 03: 0.29dB / 0.13dB
Fiber 04: 0.07dB / 0.08dB
Fiber 05: 0.08dB / 0.13dB
Fiber 06: 0.05dB / 0.05dB
Fiber 07: 0.03dB / 0.03dB
Fiber 08: 0.04dB / 0.04dB
Fiber 09: 0.31dB / 0.33dB
Fiber 10: 0.03dB / 0.03dB
Fiber 11: 0.25dB / 0.26dB
Fiber 12: 0.22dB / 0.19dB
)";
            tab->fillFiberTable(rawData);
        }
    }
}

void mainWid::on_ConfirmTpl_clicked()
{
    clearTemplateFields();
    mFiberTem->clearInfo();

    QString pn = ui->txtPN->text().trimmed();

    // 1️⃣ 检测模板号合法性
    if (!checker.verify(pn)) {
        msgCenter->sendTip("模板号不合法！",Qt::red);
        return;
    }

    // 2️⃣ 加载模板信息
    if (mFiberTem->loadFromDatabase(pn)) {

        // 3️⃣ 获取模板信息
        TemInfo info = mFiberTem->getTemInfo();
        setTemInfo();
        createFanTable();

        // 4️⃣ 打印 TemInfo 基本信息
        qDebug() << "=== 模板信息 ===";
        qDebug() << "PN:" << info.PN;
        qDebug() << "FanoutPn:" << info.FanoutPn;
        qDebug() << "光纤数量:" << info.FiberCount;
        qDebug() << "描述:" << info.description;

        // 5️⃣ 打印 FiberInfo 枚举信息
        qDebug() << "--- FiberInfo ---";
        qDebug() << "接口类型:" << static_cast<int>(info.info.iface);
        qDebug() << "光纤芯数:" << static_cast<int>(info.info.count);
        qDebug() << "光纤规格:" << static_cast<int>(info.info.spec);
        qDebug() << "极性:" << static_cast<int>(info.info.polarity);
        qDebug() << "模式:" << static_cast<int>(info.info.mode);
        msgCenter->sendTip("已加载模板信息\n\n请扫码配件1",Qt::yellow);

    } else {
        msgCenter->sendTip("未找到改模板！",Qt::red);
        return ;
    }
}

void mainWid::createFanTable()
{
    ui->tabWidget->clear();
    for(int i = 1; i <= mFiberTem->getTemInfo().FiberCount; i ++){
        FiberTab *tab = new FiberTab();
        ui->tabWidget->addTab(tab, QString("扇出线%1").arg(i));
    }
}

void mainWid::setTemInfo()
{
    TemInfo info = mFiberTem->getTemInfo();
    ui->txtProductID->setText(info.PN);
    ui->fanOutPN->setText(info.FanoutPn);
    ui->txtTemplateDesc->setText(info.description);
    ui->txtPolarityType->setText(polarityToString(info.info.polarity));
    ui->txtFiberSpec->setText(specToString(info.info.spec));
    ui->txtQuantity->setText(QString::number(info.FiberCount));
    ui->txtType->setText(ifaceToString(info.info.iface)+" "+countToString(info.info.count));

    setAccessoryVisible(info.FiberCount);
}

void mainWid::setAccessoryVisible(int count)
{
    // 所有配件相关控件打包成数组方便操作
    QList<QWidget*> accessories = {
        ui->label_17, ui->lblAccessory1Scan, ui->btnManualInput1,
        ui->label_18, ui->lblAccessory2Scan, ui->btnManualInput2,
        ui->label_19, ui->lblAccessory3Scan, ui->btnManualInput3,
        ui->label_20, ui->lblAccessory4Scan, ui->btnManualInput4
    };

    // 先全部隐藏
    for (auto w : accessories)
        w->hide();

    // 根据 count 显示对应数量的配件
    if (count >= 1) {
        ui->label_17->show();
        ui->lblAccessory1Scan->show();
        ui->btnManualInput1->show();
    }
    if (count >= 2) {
        ui->label_18->show();
        ui->lblAccessory2Scan->show();
        ui->btnManualInput2->show();
    }
    if (count >= 3) {
        ui->label_19->show();
        ui->lblAccessory3Scan->show();
        ui->btnManualInput3->show();
    }
    if (count >= 4) {
        ui->label_20->show();
        ui->lblAccessory4Scan->show();
        ui->btnManualInput4->show();
    }
}

void mainWid::getInstCon()
{
    mFiberTem = &FiberTem::instance();
    msgCenter = MsgCenter::instance();
    connect(msgCenter,&MsgCenter::tipChanged,this,[=](const QString& text, const QColor& color){
        ui->msgBoxTip->setText(text);
        QPalette pal = ui->msgBoxTip->palette();
        pal.setColor(QPalette::Window,color);
        ui->msgBoxTip->setPalette(pal);
        ui->msgBoxTip->setAutoFillBackground(true);
        // 设置字体
        QFont font;
        font.setPointSize(13);  // 字号
        font.setBold(true);      // 加粗
        ui->msgBoxTip->setFont(font);
    });
}

void mainWid::clearTemplateFields()
{
    // 清空 QLineEdit
    ui->txtType->clear();
    ui->txtFiberSpec->clear();
    ui->txtPolarityType->clear();
    ui->fanOutPN->clear();
    ui->txtQuantity->clear();
    ui->txtProductID->clear();

    // 清空 QTextEdit
    ui->txtTemplateDesc->clear();
    //ui->tabWidget->clear();
}
