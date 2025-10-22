#include "mainwid.h"
#include "ui_mainwid.h"
#include "backcolourcom.h"
#include "fibertab.h"
#include "manualinputdialog.h"

#include <QTimer>
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
    if (working) {
        auto reply = QMessageBox::question(this, tr("停止工作"),
                                           tr("当前正在工作，是否中断并停止？"),
                                           QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No) {
            QTimer::singleShot(0, scanInput, SLOT(setFocus()));
            return;
        }
        stopWork();
        return;
    }

    scanningMode = false;
    fanIndex = 0;
    totalFans = 0;
    scanInput->clear();

    QString pn = ui->txtPN->text().trimmed();
    // if (!checker.verify(pn)) {
    //     msgCenter->sendTip("模板号不合法！", Qt::red);
    //     return;
    // }
    if (!mFiberTem->loadFromDatabase(pn)) {
        msgCenter->sendTip("未找到该模板！", Qt::red);
        return;
    }
    // 启动工作（内部设置 working/scanningMode 等）
    startWork(pn);
}

void mainWid::createFanTable()
{
    ui->tabWidget->clear();
    for(int i = 1; i <= mFiberTem->getTemInfo().FanCount; i ++){
        FiberTab *tab = new FiberTab();
        ui->tabWidget->addTab(tab, QString("扇出线%1").arg(i));
    }
}

void mainWid::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    activePage = true;
    if (scanningMode && working) {
        scanInput->clear();
        scanInput->setFocus();
    }
}

void mainWid::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    activePage = false;
    // 离开页面时不强制清理 working，但扫码焦点不可用
}

bool mainWid::eventFilter(QObject *obj, QEvent *event)
{
    // 在工作并处于扫码模式且页面可见时，保持扫码焦点
    if (activePage && scanningMode && working) {
        if (event->type() == QEvent::MouseButtonPress ||
            event->type() == QEvent::FocusIn ||
            event->type() == QEvent::KeyPress) {

            QWidget *fw = QApplication::focusWidget();
            if (fw && fw != scanInput) {
                QTimer::singleShot(50, this, [this]() {
                    if (activePage && scanningMode && working) {
                        scanInput->setFocus(Qt::OtherFocusReason);
                    }
                });
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

bool mainWid::checkFiberInTem(ScanInfo& info)
{
    sTemInfo Tem = mFiberTem->getTemInfo();

    // struct sTemInfo {
    //     QString PN;             // 模板编码
    //     QString FanoutPn;       // 扇出线物料编码
    //     int FanCount = 1;       // 扇出线数量
    //     double limit = 0.0;     // 最大插入损耗
    //     QString description;    // 模板描述
    //     sFiberInfo info;        // 光纤信息

    //     std::pair<int,int>lambda; //

    //     QString qrTemplate;     // 二维码模板    --TemplateA(B、C)
    //     QString labelTemplate;  // 标签模板ID    --Template1(2、3)
    // };

    // struct ScanInfo {
    //     QString id;          // 扫描 ID
    //     QString serNum;      // 序列号
    //     int count;           // 光纤数量
    //     QString wavelength;  // 波长，例如 1310nm/1550nm
    //     QString standard;    // 标准，例如 0.50dB
    //     QString rawData;     // Fiber 原始数据，用于填充 FiberTab
    //     QString QRCodeContent;// 原二维码内容
    // };

    if (Tem.FanoutPn != info.id) {
        qDebug() << "[Mismatch] FanoutPn 不匹配:"
                 << "模板=" << Tem.FanoutPn
                 << "扫码=" << info.id;
        return false;
    }

    if (static_cast<int>(Tem.info.count) != info.count) {
        qDebug() << "[Mismatch] FanCount 不匹配:"
                 << "模板=" << static_cast<int>(Tem.info.count)
                 << "扫码=" << info.count;
        return false;
    }

    QString wave;
    if (Tem.info.mode == FiberMode::SM) wave = "1310nm/1550nm";
    else if (Tem.info.mode == FiberMode::MM) wave = "850nm/1300nm";

    if (wave != info.wavelength) {
        qDebug() << "[Mismatch] 波长不匹配:"
                 << "模板=" << wave
                 << "扫码=" << info.wavelength;
        return false;
    }

    if (Tem.limit < info.ilimit) {
        qDebug() << "[Mismatch] IL限值不匹配:"
                 << "模板限值=" << Tem.limit
                 << "扫码=" << info.ilimit;
        return false;
    }

    if (checkFiberLosses(info)) {
        qDebug() << "[Mismatch] 光纤损耗超出限值!";
        return false;
    }

    return true;

}

void mainWid::handleScanCode(const QString &code)
{
    qDebug()<<code;
    // 只有在工作流程中并处于扫码模式并且页面可见才接收扫码
    if (!working || !scanningMode || !activePage) {
        qDebug() << "忽略扫码（非工作状态或页面不可见）:" << code;
        return;
    }

    if (code.isEmpty()) return;

    qDebug() << "扫码" << (fanIndex + 1) << ":" << code;

    ui->tabWidget->setCurrentIndex(fanIndex);
    FiberTab *tab = qobject_cast<FiberTab*>(ui->tabWidget->widget(fanIndex));
    ScanInfo info = parseScanDataNewFormat(code);  // 用扫码内容解析

    // 调整顺序
    QStringList fibers = info.rawData.split('\n', Qt::SkipEmptyParts);
    info.rawData = reorderFibers(fibers, info.count, polarityToString(mFiberTem->getTemInfo().info.polarity)).join("\n");

    bool ok = checkFiberInTem(info);

//    bool ok = true;
    if (ok) {
        tab->setInfo(info.serNum, info.count, info.wavelength, info.standard);
        tab->fillFiberTable(info.rawData);

        int fibersPerFan = info.count;                 // 每个扇出线光纤数量
        int fiberStartIndex = fanIndex * fibersPerFan; // 当前扇出线起始编号

        QStringList fiberLines = info.rawData.split('\n', Qt::SkipEmptyParts);
        QStringList lossesTextList;

        for (int i = 0; i < fiberLines.size(); ++i) {
            QString loss = fiberLines[i].split(":").last().trimmed();
            // 处理双值情况，比如 "0.23/0.21"
            QStringList vals = loss.split('/');
            if (vals.size() == 2) {
                loss = QString("%1dB / %2dB")
                .arg(vals[0].trimmed())
                    .arg(vals[1].trimmed());
            } else {
                loss += " dB";
            }

            lossesTextList << QString("Fiber %1: %2")
                                  .arg(fiberStartIndex + i + 1, 2, 10, QChar('0'))
                                  .arg(loss);
        }

        // 注意 append 会自动换行，如果不想最后换行可用 setText 或 remove 最后一个换行
        ui->lblQRCodeInfo->append(lossesTextList.join("\n"));
    }

    if (!ok) {
        msgCenter->sendTip(QString("配件 %1/%2 扫码失败，请重新扫码！")
                               .arg(fanIndex + 1)
                               .arg(totalFans), Qt::red);
        scanInput->clear();
        scanInput->setFocus();
        return; // 不前进
    }

    // 解析成功 -> 显示到对应控件
    /* code exmple
    "A031375AA;25W40;45019237250400001;1310nm/1550nm;<0.5dB:PASS;
    F01:0.23/0.21;F02:0.32/0.35;F03:0.41/0.42;F04:0.35/0.38;F05:0.41/0.44;F06:0.32/0.36;F07:0.38/0.39;F08:0.42/0.41"
    */

    switch (fanIndex) {
    case 0: ui->lblAccessory1Scan->setText(code); log.seq1 = code.split(';').value(2); log.qr1 = code; break;
    case 1: ui->lblAccessory2Scan->setText(code); log.seq2 = code.split(';').value(2); log.qr2 = code; break;
    case 2: ui->lblAccessory3Scan->setText(code); log.seq3 = code.split(';').value(2); log.qr3 = code; break;
    case 3: ui->lblAccessory4Scan->setText(code); log.seq4 = code.split(';').value(2); log.qr4 = code; break;
    default: /* 如果超过，则忽略或保存到表 */ break;
    }

    fanIndex++;
    if (fanIndex < totalFans) {
        msgCenter->sendTip(QString("请扫码配件 %1/%2")
                               .arg(fanIndex + 1)
                               .arg(totalFans), Qt::yellow);
    } else {

        msgCenter->sendTip(" 所有配件扫码完成！", Qt::green);
        log.qrContent = ui->lblQRCodeInfo->toPlainText();
        if (dblog->insertItem(log)) {
            qDebug() << "日志插入成功, ID =" << log.id;
            mgr->saveCurrentNum();

            sLabelInfo info;
            info.desc = log.description;
            info.qr = log.qrContent;
            info.PN = log.PN;
            QDate today = QDate::currentDate();
            int yy = today.year() % 100;          // 年份后两位
            int ww = today.weekNumber();          // 周数
            QString yyWww = QString("%1W%2")
                                .arg(yy, 2, 10, QChar('0'))
                                .arg(ww, 2, 10, QChar('0')); // 年周
            info.date = yyWww;
            emit doprint(info);
        } else {
            qDebug() << "日志插入失败";
        }
        stopWork();
    }

    scanInput->clear();
    // 仅当仍在工作且页面可见时继续聚焦
    if (working && scanningMode && activePage)
        scanInput->setFocus();
}

void mainWid::startWork(const QString &pn)
{
    // 保护重入：先确保已清理旧状态
    ui->txtPN->setEnabled(false);
    scanningMode = false;
    fanIndex = 0;
    totalFans = 0;
    scanInput->clear();

    sTemInfo info = mFiberTem->getTemInfo();
    log = sFiberLogItem();

    setTemInfo();
    createFanTable();

    totalFans = info.FanCount;
    fanIndex = 0;
    scanningMode = true;
    working = true;


    log.boxId = mgr->generateFullCode(info.FanoutPn);
    log.PN = info.PN;
    log.description = info.description;
    log.fanoutPn = info.FanoutPn;
    log.limitIL = info.limit;
    log.fanoutCount = info.FanCount;
    log.waveType = modeToString(info.info.mode);


    ui->lblQRCodeInfo->clear();
    QString infoText;
    infoText += QString("ID: %1\n").arg(log.boxId);
    infoText += QString("Fiber Insertion Losses\n");
    infoText += QString("(%1)(<%2dB):%3")
                    .arg(modeToWaveLength(info.info.mode))
                    .arg(info.limit, 0, 'f', 2)
                    .arg("PASS");

    // 写入界面标签
    ui->lblQRCodeInfo->setText(infoText);



    ui->ConfirmTpl->setText(tr("停止工作"));
    // 可选：改变样式区分状态
    ui->ConfirmTpl->setStyleSheet("background-color: #d9534f; color: white;");

    msgCenter->sendTip(QString("模板 %1 已加载\n\n请扫码配件 1/%2")
                           .arg(pn)
                           .arg(totalFans),
                       Qt::yellow);

    // 启动扫码焦点（仅当界面可见）
    if (activePage) {
        scanInput->clear();
        scanInput->setFocus();
    }
}

void mainWid::stopWork()
{
    // 停止工作并清理状态
    working = false;
    scanningMode = false;
    fanIndex = 0;
    totalFans = 0;
    scanInput->clear();
    ui->txtPN->setEnabled(true);
    // 恢复按钮文本及样式
    ui->ConfirmTpl->setText(tr("确认模板"));
    ui->ConfirmTpl->setStyleSheet(""); // 恢复默认样式

    msgCenter->sendTip("工作已停止。\n\n 请重新确认物料编码", Qt::red);
    setAccessoryVisible(0);
    // 可选：清空配件显示
    ui->lblAccessory1Scan->clear();
    ui->lblAccessory2Scan->clear();
    ui->lblAccessory3Scan->clear();
    ui->lblAccessory4Scan->clear();
}


void mainWid::init()
{
    // 创建隐藏扫码输入框
    scanInput = new QLineEdit(this);
    scanInput->setObjectName("lineEditScan");
    //scanInput->hide();
    scanInput->setFocusPolicy(Qt::StrongFocus); // 强焦点策略

    connect(scanInput, &QLineEdit::returnPressed, this, [=]() {
        QString code = scanInput->text().trimmed();
        scanInput->clear();
        handleScanCode(code);
    });

    // 安装事件过滤器（用于扫码期间锁焦）
    this->installEventFilter(this);

    // 初始状态
    working = false;
    scanningMode = false;
    activePage = false;
    fanIndex = 0;
    totalFans = 0;

    // 把按钮文本设置为“确认模板”。注意按钮对象名是 ui->ConfirmTpl
    ui->ConfirmTpl->setText(tr("确认模板"));

    // 其它原有初始化
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
    mFiberTem   = &FiberTem::instance();
    msgCenter   = MsgCenter::instance();
          mgr   = SerialMgr::instance();
        dblog   =    DbLogs::instance();
    printThread = new printworker(this);

    connect(this,&mainWid::doprint,printThread,&printworker::doprint);


    connect(msgCenter,&MsgCenter::tipChanged,this,[=](const QString& text, const QColor& color){
        ui->msgBoxTip->setText(text);
        QPalette pal = ui->msgBoxTip->palette();
        pal.setColor(QPalette::Window,color);
        ui->msgBoxTip->setPalette(pal);
        ui->msgBoxTip->setAutoFillBackground(true);

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
    ui->txtTemplateDesc->clear();
}

void mainWid::exmple()
{
    msgCenter->sendTip("Test", Qt::red);
    ui->tabWidget->clear();

    // 创建多个扇出线 Tab
    for (int i = 1; i <= 3; ++i) {
        FiberTab *tab = new FiberTab();
        ui->tabWidget->addTab(tab, QString("扇出线%1").arg(i));
    }


    // 模拟不同扇出线的扫码数据
    QStringList scanDatas = {
        "A031375AA;25W40;45019237250400001;1310nm/1550nm;<0.5dB:PASS;F01:0.23/0.21;F02:0.32/0.35;F03:0.41/0.42;F04:0.35/0.38;F05:0.41/0.44;F06:0.32/0.36;F07:0.38/0.39;F08:0.42/0.41;F09:0.23/0.21;F10:0.32/0.35;F11:0.41/0.42;F12:0.35/0.38;F13:0.41/0.44;F14:0.32/0.36;F15:0.38/0.39;F16:0.42/0.41",
        "B045678BB;30W50;45019237250400002;1310nm/1550nm;<0.5dB:PASS;F01:1.25/0.22;F02:0.34/0.36;F03:1.25/0.22;F04:0.34/0.36;F05:0.41/0.44;F06:0.32/0.36;F07:0.38/0.39;F08:0.34/0.36",
        "C059999CC;40W60;45019237250400003;1310nm/1550nm;<0.5dB:PASS;F01:0.21/0.20;F02:0.30/0.31;F03:1.25/0.22;F04:0.34/0.36;F05:0.41/0.44;F06:0.32/0.36;F07:0.38/0.39;F08:0.42/0.41;F09:0.23/0.21;F10:0.32/0.35;F11:0.41/0.42;F12:0.35/0.38"
    };

    for (int i = 0; i < ui->tabWidget->count(); ++i) {

        FiberTab *tab = qobject_cast<FiberTab*>(ui->tabWidget->widget(i));
        if (tab && i < scanDatas.size()) {
            ScanInfo info = parseScanDataNewFormat(scanDatas[i]);
            tab->setInfo(info.serNum, info.count, info.wavelength, info.standard);

            tab->fillFiberTable(info.rawData);
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

void mainWid::handleManualInput(int index)
{
    ManualInputDialog dlg(this);
    auto info = mFiberTem->getTemInfo();

    dlg.setTemplateInfo(info.FanoutPn,
                        static_cast<int>(info.info.mode),
                        static_cast<int>(info.info.count));

    if (dlg.exec() == QDialog::Accepted) {
        QString code = dlg.getScanCode();
        handleScanCode(code);
    } else {
        QTimer::singleShot(0, scanInput, SLOT(setFocus()));
    }
}

void mainWid::on_btnManualInput1_clicked() { handleManualInput(1); }

void mainWid::on_btnManualInput2_clicked() { handleManualInput(2); }

void mainWid::on_btnManualInput3_clicked() { handleManualInput(3); }

void mainWid::on_btnManualInput4_clicked() { handleManualInput(4); }


// // 4️⃣ 打印 TemInfo 基本信息
// qDebug() << "=== 模板信息 ===";
// qDebug() << "PN:" << info.PN;
// qDebug() << "FanoutPn:" << info.FanoutPn;
// qDebug() << "光纤数量:" << info.FanCount;
// qDebug() << "描述:" << info.description;

// // 5️⃣ 打印 FiberInfo 枚举信息
// qDebug() << "--- FiberInfo ---";
// qDebug() << "接口类型:" << static_cast<int>(info.info.iface);
// qDebug() << "光纤芯数:" << static_cast<int>(info.info.count);
// qDebug() << "光纤规格:" << static_cast<int>(info.info.spec);
// qDebug() << "极性:" << static_cast<int>(info.info.polarity);
// qDebug() << "模式:" << static_cast<int>(info.info.mode);


