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
    init();
    //exmple();
}

mainWid::~mainWid()
{
    delete ui;
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
        sTemInfo info = mFiberTem->getTemInfo();
        setTemInfo();
        createFanTable();
        fanIndex = 0;
        totalFans = info.FanCount;
        msgCenter->sendTip(QString("已加载模板信息\n\n请扫码配件 %1/%2")
                               .arg(fanIndex + 1)
                               .arg(totalFans),
                           Qt::yellow);

        // 4️⃣ 打印 TemInfo 基本信息
        qDebug() << "=== 模板信息 ===";
        qDebug() << "PN:" << info.PN;
        qDebug() << "FanoutPn:" << info.FanoutPn;
        qDebug() << "光纤数量:" << info.FanCount;
        qDebug() << "描述:" << info.description;

        // 5️⃣ 打印 FiberInfo 枚举信息
        qDebug() << "--- FiberInfo ---";
        qDebug() << "接口类型:" << static_cast<int>(info.info.iface);
        qDebug() << "光纤芯数:" << static_cast<int>(info.info.count);
        qDebug() << "光纤规格:" << static_cast<int>(info.info.spec);
        qDebug() << "极性:" << static_cast<int>(info.info.polarity);
        qDebug() << "模式:" << static_cast<int>(info.info.mode);

    } else {
        msgCenter->sendTip("未找到该模板！",Qt::red);
        return ;
    }
}

void mainWid::createFanTable()
{
    ui->tabWidget->clear();
    for(int i = 1; i <= mFiberTem->getTemInfo().FanCount; i ++){
        FiberTab *tab = new FiberTab();
        ui->tabWidget->addTab(tab, QString("扇出线%1").arg(i));
    }
}

void mainWid::handleScanCode(const QString &code)
{
    if (code.isEmpty())
        return;

    qDebug() << "扫码" << (fanIndex + 1) << ":" << code;

    // 调用解析逻辑
    //bool ok = mFiberTem->parseCode(code);
    bool ok = true;
    if (!ok) {
        // 解析失败，不增加 fanIndex，提示重扫
        msgCenter->sendTip(QString("配件 %1/%2 扫码失败，请重新扫码！")
                               .arg(fanIndex + 1)
                               .arg(totalFans), Qt::red);
        scanInput->clear();
        scanInput->setFocus();
        return;
    }

    // 解析成功，将条码显示到对应 QLabel
    switch(fanIndex) {
    case 0: ui->lblAccessory1Scan->setText(code); break;
    case 1: ui->lblAccessory2Scan->setText(code); break;
    case 2: ui->lblAccessory3Scan->setText(code); break;
    case 3: ui->lblAccessory4Scan->setText(code); break;
    }

    fanIndex++;
    if (fanIndex < totalFans) {
        msgCenter->sendTip(QString("请扫码配件 %1/%2")
                               .arg(fanIndex + 1)
                               .arg(totalFans), Qt::yellow);
    } else {
        msgCenter->sendTip("✅ 所有配件扫码完成！", Qt::green);
    }

    scanInput->clear();
    scanInput->setFocus();
}


void mainWid::init()
{
    scanInput = new QLineEdit(this);
    scanInput->setObjectName("lineEditScan");
    scanInput->hide();
    connect(scanInput, &QLineEdit::returnPressed, this, [=]() {
        QString code = scanInput->text().trimmed();
        scanInput->clear();
        handleScanCode(code);
    });

    setAccessoryVisible(0);
    ui->tabWidget->clear();
    for(int i = 1; i <= 1; i ++){
        FiberTab *tab = new FiberTab();
        ui->tabWidget->addTab(tab, QString("扇出线%1").arg(i));
    }
}

void mainWid::setTemInfo()
{
    sTemInfo info = mFiberTem->getTemInfo();
    ui->txtProductID->setText(info.PN);
    ui->fanOutPN->setText(info.FanoutPn);
    ui->txtTemplateDesc->setText(info.description);
    ui->txtPolarityType->setText(polarityToString(info.info.polarity));
    ui->txtFiberSpec->setText(specToString(info.info.spec));
    ui->txtQuantity->setText(QString::number(info.FanCount));
    ui->txtType->setText(ifaceToString(info.info.iface)+" "+countToString(info.info.count));

    setAccessoryVisible(info.FanCount);
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
    msgCenter->sendTip("请确认模板",Qt::yellow);
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

void mainWid::setAccessoryVisible(int count)
{
    // 每个配件的控件打包成三元组（Label、扫描显示、手动输入按钮）
    struct AccessoryWidgets {
        QWidget* label;
        QWidget* scanLabel;
        QWidget* manualBtn;
    };

    QVector<AccessoryWidgets> accessories = {
                                             {ui->label_17, ui->lblAccessory1Scan, ui->btnManualInput1},
                                             {ui->label_18, ui->lblAccessory2Scan, ui->btnManualInput2},
                                             {ui->label_19, ui->lblAccessory3Scan, ui->btnManualInput3},
                                             {ui->label_20, ui->lblAccessory4Scan, ui->btnManualInput4},
                                             };

    for (int i = 0; i < accessories.size(); ++i) {
        bool visible = (i < count);
        accessories[i].label->setVisible(visible);
        accessories[i].scanLabel->setVisible(visible);
        accessories[i].manualBtn->setVisible(visible);
    }
}
