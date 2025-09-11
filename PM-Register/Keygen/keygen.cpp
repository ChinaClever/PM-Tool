#include "keygen.h"
#include "ui_keygen.h"
#include "PaddingUtils.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include "qaesencryption.h"
#include <QJsonArray>
#include <QRegularExpression>
#include "backcolourcom.h"
#include <QCryptographicHash>
#include <QClipboard>
#include <QCheckBox>
#include <QHBoxLayout>
#include "SettingsHelper.h"
#include "cfgcom.h"
#include "datapacket.h"

extern QString user_land_name();
Keygen::Keygen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Keygen)
{
    ui->setupUi(this);
    m_aesItf = sAesItf(); // 使用默认构造，初始化AES参数
    mCoreThread = new Test_CoreThread(this);
    set_background_icon(this,":/image/box_back.jpg");
    m_settingsWidget = new ServiceSettingWidget(ui->settingsBox);
    ui->settingsBox->setTitle("AES加密设置");

    // 加载配置
    QSettings settings(CfgCom::bulid()->pathOfData("cfg.ini"), QSettings::IniFormat);

    m_settingsWidget->loadSettings(settings);
    connect(m_settingsWidget, &ServiceSettingWidget::configChanged,
            this, &Keygen::onSettingsConfigChanged);
    init();

}

Keygen::~Keygen()
{
    delete ui;
}

void Keygen::onSettingsConfigChanged(const sAesItf &config, const QString &savePath)
{
    setAesConfig(config);
    setSaveDir(savePath);

    // 保存配置到文件
    QSettings settings(CfgCom::bulid()->pathOfData("cfg.ini"), QSettings::IniFormat);

    m_settingsWidget->saveSettings(settings);
}

void Keygen::init()
{

    buttonGroup = new QButtonGroup(this);

    buttonGroup->addButton(ui->verOnBtn,0);
    buttonGroup->addButton(ui->verOffBtn,1);

    ui->dateEditExpire->setCalendarPopup(true);
    ui->tableFeatures->setColumnCount(2);
    ui->tableFeatures->setHorizontalHeaderLabels(QStringList() << "启用" << "功能名称");
    ui->tableFeatures->setColumnWidth(1, 585);
    ui->dateEditExpire->setDisplayFormat("yyyy-MM-dd");
    ui->dateEditExpire->setDate(QDate::currentDate());
    m_authFilePath.clear();
    ui->lineEditFilePath->setReadOnly(true);
}

QByteArray Keygen::aesEncrypt(const QByteArray &plainText)
{
    QAESEncryption encryption(m_aesItf.level, m_aesItf.mode, m_aesItf.padding);
    QByteArray encrypted = encryption.encode(plainText, m_aesItf.key, m_aesItf.iv);
    if (encrypted.isEmpty()) {
        qWarning() << "AES加密失败";
    }
    return encrypted;
}

QByteArray Keygen::aesDecrypt(const QByteArray &encryptedData)
{
    QAESEncryption decryption(m_aesItf.level, m_aesItf.mode, m_aesItf.padding);
    QByteArray decrypted = decryption.decode(encryptedData, m_aesItf.key, m_aesItf.iv);
   // qDebug()<<m_aesItf.key<<' '<<m_aesItf.iv;
    if (decrypted.isEmpty()) {
        qWarning() << "AES解密失败";
    }
    return decrypted;
}

void Keygen::saveEncryptedFile(const QByteArray &data, const QString &fileName)
{
    QString filePath;
    if (!m_saveDir.isEmpty()) {
        // m_saveDir 是目录，fileName 是文件名
        filePath = QDir(m_saveDir).filePath(fileName);
    } else {
        QDir dir(QCoreApplication::applicationDirPath());
        QString dirName = "RegisterFiles";
        if (!dir.exists(dirName)) dir.mkdir(dirName);
        dir.cd(dirName);
        filePath = dir.filePath(fileName);
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::warning(this, "错误", "无法打开文件写入：" + filePath);
        return;
    }
    file.write(data);
    file.close();

    QMessageBox::information(this, "成功", "授权文件生成成功！\n路径：" + filePath);
    qDebug() << "文件已保存：" << filePath;

    mDev->dt.licenseFile = data;
    setDevlogs();
}

void Keygen::setDevlogs()
{
    mDev->dt.user = user_land_name();
    mDev->dt.keylength    = AESHelper::levelToStr(m_aesItf.level);
    mDev->dt.encryption = AESHelper::modeToStr(m_aesItf.mode);
    mDev->dt.paddingMode  = AESHelper::paddingToStr(m_aesItf.padding);
    mDev->dt.iv = m_aesItf.iv.toHex();
    mDev->dt.key = m_aesItf.key.toHex();
    mDev->dt.sn = ui->Sntext->toPlainText();
    mDev->dt.customer = ui->lineEditCustomer->text();
    mDev->dt.dateEnd = ui->dateEditExpire->text();
    //qDebug()<<mDev->dt.dateEnd;

    mCoreThread->start();
}

QByteArray Keygen::loadEncryptedFile() //读取授权文件内容信息
{
    if (m_authFilePath.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择授权文件");
        return QByteArray();
    }

    QByteArray encryptedData;
    {
        QFile file(m_authFilePath);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::warning(this, "错误", "无法打开文件：" + m_authFilePath);
            return QByteArray();
        }
        encryptedData = file.readAll();
        // 离开作用域自动析构 + close
    }
    return encryptedData;
}

void Keygen::on_generateButton_clicked()//生成许可证
{
    mPacket->init(); // 初始化日志对象

    // QJsonObject featuresObj{
    //     {"全部权限", true},
    //     {"部分功能", true},
    //     {"其他功能", true}
    // };
    // QJsonObject jsonObj{
    //     {"auth_required", true},
    //     {"device_sn", "SN987654"},
    //     {"expire_date", ""},
    //     {"customer", QString::fromUtf8("lukangmi21ng")},
    //     {"activation_code", QString::fromUtf8("")},
    //     {"features", featuresObj}  // 改成对象形式
    // };

    if (ui->Sntext->toPlainText().isEmpty()) {
        QMessageBox::warning(this, "提示", "请先导入授权文件");
        return;
    }

    int rowCount = ui->tableFeatures->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        QString featureName = ui->tableFeatures->item(i, 1)->text();
        QWidget *widget = ui->tableFeatures->cellWidget(i, 0);
        QCheckBox *checkBox = widget->findChild<QCheckBox *>();
        bool enabled = checkBox && checkBox->isChecked();
        featuresObj[featureName] = enabled;
    }
    jsonObj["features"] = featuresObj;
    jsonObj["expire_date"] = ui->dateEditExpire->text();
    jsonObj["customer"] = ui->lineEditCustomer->text();

    if(buttonGroup->button(0)->isCheckable()) jsonObj["auth_required"] = "true";
    else jsonObj["auth_required"] = "false";

    QString combined = jsonObj.value("device_sn").toString()
                       + jsonObj["expire_date"].toString()
                       + jsonObj["customer"].toString();

    QByteArray hash = QCryptographicHash::hash(combined.toUtf8(), QCryptographicHash::Sha256);
    QString activation_code = hash.toHex();

    mDev->dt.activationCode = activation_code;

   // qDebug()<<activation_code;
    ui->actiCodeText->setText(activation_code);
    jsonObj["activation_code"] = activation_code;

    QJsonDocument doc(jsonObj);
    QByteArray plainText = doc.toJson(QJsonDocument::Compact);

    QByteArray encryptedData = aesEncrypt(plainText);
    if (encryptedData.isEmpty()) {
        QMessageBox::critical(this, "错误", "加密失败");
        return;
    }

    QByteArray base64Data = encryptedData.toBase64();

    QString customer = jsonObj.value("customer").toString().trimmed();
    QString expireDate = jsonObj.value("expire_date").toString().trimmed();
    QString expireFlag = (expireDate == "2099-12-31") ? "PERM" : expireDate;
    customer.replace(QRegularExpression("[\\\\/:*?\"<>|\\s]"), "_");expireFlag.replace(QRegularExpression("[\\\\/:*?\"<>|\\s]"), "_");

    QString fileName = QString("%1_%2.license").arg(customer, expireFlag);
    saveEncryptedFile(base64Data, fileName);
}

void Keygen::on_openFileButton_clicked()//导入并解析授权文件
{
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    tr("选择授权文件"),
                                                    QCoreApplication::applicationDirPath(),
                                                    tr("授权文件 (*.auth);"));
    if (filePath.isEmpty())
        return;
    m_authFilePath = filePath;
    ui->lineEditFilePath->setText(filePath);
    decryptFile();
}

void Keygen::decryptFile() //读取并解密文件
{
    QByteArray encryptedBase64Data = loadEncryptedFile();
    if (encryptedBase64Data.isEmpty()) {
        return;
    }

    QByteArray encryptedData = QByteArray::fromBase64(encryptedBase64Data);


    QByteArray decryptedData = aesDecrypt(encryptedData);
    if (decryptedData.isEmpty()) {
        QMessageBox::critical(this, "错误", "解密失败，密钥/IV可能错误或文件被破坏");
        return;
    }

    if (m_aesItf.padding == 1)
        decryptedData = PaddingUtils::removePKCS7Padding(decryptedData);
    else if (m_aesItf.padding == 0)
        decryptedData = PaddingUtils::removeZeroPadding(decryptedData);
    else
        decryptedData = PaddingUtils::removeISO7816Padding(decryptedData);


    m_authFilePath.clear();
    ui->lineEditFilePath->clear();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(decryptedData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
       // QMessageBox::critical(this, "错误", "JSON解析失败: " + parseError.errorString());
        QMessageBox::critical(this, "错误", "授权文件损坏或格式错误，请检查格式是否正确");
       // qDebug() << "解密后原始数据:" << decryptedData;
        return;
    }

    jsonObj = doc.object();
    QJsonObject featuresObj = jsonObj["features"].toObject();
    ui->tableFeatures->setRowCount(featuresObj.size());
    int row = 0;
    for (auto it = featuresObj.begin(); it != featuresObj.end(); ++it, ++row) {
        QString featureName = it.key();
        bool enabled = it.value().toBool();

        QTableWidgetItem *nameItem = new QTableWidgetItem(featureName);
        nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
        nameItem->setTextAlignment(Qt::AlignCenter); // 字体居中
        ui->tableFeatures->setItem(row, 1, nameItem);

        // 用QCheckBox居中显示
        QCheckBox *checkBox = new QCheckBox();
        checkBox->setChecked(enabled);

        QWidget *widget = new QWidget();
        QHBoxLayout *layout = new QHBoxLayout(widget);
        layout->addWidget(checkBox);
        layout->setAlignment(checkBox, Qt::AlignCenter);
        layout->setContentsMargins(0, 0, 0, 0);
        widget->setLayout(layout);

        ui->tableFeatures->setCellWidget(row, 0, widget);
    }

    //qDebug() << "解密后的JSON:" << jsonObj;
    if(jsonObj.value("auth_required") == "true") buttonGroup->button(0)->setChecked(1);
    else buttonGroup->button(1)->setChecked(1);

    ui->lineEditCustomer->setText(jsonObj.value("customer").toString());
    ui->Sntext->setText(jsonObj.value("device_sn").toString());
    ui->dateEditExpire->setDate(QDate::fromString(jsonObj.value("expire_date").toString(), "yyyy-MM-dd"));
    QMessageBox::information(this, "成功", "解密成功");
}

void Keygen::on_tiralBtn_clicked()
{
    ui->dateEditExpire->setDisplayFormat("yyyy-MM-dd");
    ui->dateEditExpire->setDate(QDate::currentDate().addDays(100)); // 当前时间加15天
}

void Keygen::on_formBtn_clicked()
{
    ui->dateEditExpire->setDisplayFormat("yyyy-MM-dd");
    ui->dateEditExpire->setDate(QDate::currentDate().addYears(10)); // 当前时间加10年
}

void Keygen::on_copyBtn_clicked() //复制激活码槽函数
{
    QString textToCopy = ui->actiCodeText->toPlainText();
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(textToCopy);
    QMessageBox::information(this, "提示", "激活码已复制到剪贴板！");
}

void Keygen::setAesConfig(const sAesItf &config)
{
    m_aesItf = config;
    // qDebug() << "已更新 AES 配置:"
    //          << "Level:" << m_aesItf.level
    //          << "Mode:" << m_aesItf.mode
    //          << "Padding:" << m_aesItf.padding
    //          << "Key:" << m_aesItf.key.toHex()
    //          << "IV:" << m_aesItf.iv.toHex();
}

void Keygen::setSaveDir(const QString &dir)
{
    m_saveDir = dir;
}

void Keygen::loadSettings(QSettings &settings)
{
    m_saveDir = settings.value("saveDir", "").toString();
    SettingsHelper::loadAesItf(settings, m_aesItf);

}

