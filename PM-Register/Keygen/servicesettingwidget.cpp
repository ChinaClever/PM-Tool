// servicesettingwidget.cpp
#include "servicesettingwidget.h"
#include "ui_servicesettingwidget.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
ServiceSettingWidget::ServiceSettingWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ServiceSettingWidget)
{
    ui->setupUi(this);
    ui->saveDir->setReadOnly(true);
    QSettings settings(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);

    initDialog();
    setStatus(1);

}

ServiceSettingWidget::~ServiceSettingWidget()
{
    delete ui;
}

void ServiceSettingWidget::initDialog()
{
    ui->comboBoxLevel->addItem("AES 128", QAESEncryption::AES_128);
    ui->comboBoxLevel->addItem("AES 192", QAESEncryption::AES_192);
    ui->comboBoxLevel->addItem("AES 256", QAESEncryption::AES_256);
    ui->comboBoxLevel->setCurrentIndex(2);

    ui->comboBoxMode->addItem("ECB", QAESEncryption::ECB);
    ui->comboBoxMode->addItem("CBC", QAESEncryption::CBC);
    ui->comboBoxMode->addItem("CFB", QAESEncryption::CFB);
    ui->comboBoxMode->addItem("OFB", QAESEncryption::OFB);
    ui->comboBoxMode->setCurrentIndex(1);

    ui->comboBoxPadding->addItem("ZERO", QAESEncryption::ZERO);
    ui->comboBoxPadding->addItem("PKCS7", QAESEncryption::PKCS7);
    ui->comboBoxPadding->addItem("ISO7816", QAESEncryption::ISO);
    ui->comboBoxPadding->setCurrentIndex(2);

    m_config = sAesItf();
    ui->lineEditKey->setText(QString(m_config.key.toHex()));
    ui->lineEditIv->setText(QString(m_config.iv.toHex()));

    m_tempConfig = m_config;
}

void ServiceSettingWidget::updateUIFromConfig(const sAesItf &config)
{
    ui->comboBoxLevel->setCurrentIndex(ui->comboBoxLevel->findData(config.level));
    ui->comboBoxMode->setCurrentIndex(ui->comboBoxMode->findData(config.mode));
    ui->comboBoxPadding->setCurrentIndex(ui->comboBoxPadding->findData(config.padding));
    ui->lineEditKey->setText(QString(config.key.toHex()));
    ui->lineEditIv->setText(QString(config.iv.toHex()));
}

void ServiceSettingWidget::on_applyBtn_clicked()
{

    if(ui->applyBtn->text() == "修改"){
        ui->applyBtn->setText("保存");
        setStatus(false);
        // 如果是修改模式，显示真实的密钥和IV
        ui->lineEditKey->setText(QString(m_tempConfig.key.toHex()));
        ui->lineEditIv->setText(QString(m_tempConfig.iv.toHex()));

    }

    else{
        // 从界面读取临时配置
        m_tempConfig.level = static_cast<QAESEncryption::Aes>(ui->comboBoxLevel->currentData().toInt());
        m_tempConfig.mode = static_cast<QAESEncryption::Mode>(ui->comboBoxMode->currentData().toInt());
        m_tempConfig.padding = static_cast<QAESEncryption::Padding>(ui->comboBoxPadding->currentData().toInt());
        m_tempConfig.key = QByteArray::fromHex(ui->lineEditKey->toPlainText().toUtf8());
        m_tempConfig.iv = QByteArray::fromHex(ui->lineEditIv->toPlainText().toUtf8());

        // 校验逻辑
        bool validKeyLength = false;
        switch (m_tempConfig.level) {
        case QAESEncryption::AES_128: validKeyLength = (m_tempConfig.key.size() == 16); break;
        case QAESEncryption::AES_192: validKeyLength = (m_tempConfig.key.size() == 24); break;
        case QAESEncryption::AES_256: validKeyLength = (m_tempConfig.key.size() == 32); break;
        default: validKeyLength = false;
        }
        //qDebug()<<m_tempConfig.key.size();
        if (!validKeyLength) {
            QMessageBox::warning(this, "错误", "密钥长度不正确，请输入对应长度的16/24/32字节密钥（十六进制字符串）");
            return;
        }

        if (m_tempConfig.mode != QAESEncryption::ECB && m_tempConfig.iv.size() != 16) {
            QMessageBox::warning(this, "错误", "IV长度必须为16字节（十六进制字符串），ECB模式下可为空");
            return;
        }

        // 应用配置
        m_config = m_tempConfig;
        m_savePath = m_tempSavePath;
        // 发送配置改变信号
        emit configChanged(m_config, m_savePath);
        QMessageBox::information(this, "提示", "配置已应用");

        ui->applyBtn->setText("修改");
        ui->lineEditKey->setPlainText("**********");
        ui->lineEditIv->setPlainText("**********");
        setStatus(true);
    }

}

void ServiceSettingWidget::setStatus(bool flag)
{
    // 设置所有控件为不可用（禁用）状态
    ui->changeDirBtn->setEnabled(!flag);   // 更改路径按钮
    ui->resetBtn->setEnabled(!flag);       // 重置按钮
    ui->comboBoxPadding->setEnabled(!flag); // Padding ComboBox
    ui->comboBoxLevel->setEnabled(!flag);  // Level ComboBox
    ui->comboBoxMode->setEnabled(!flag);   // Mode ComboBox
    ui->lineEditKey->setEnabled(!flag);    // Key TextEdit
    ui->lineEditIv->setEnabled(!flag);     // IV TextEdit
}

void ServiceSettingWidget::on_resetBtn_clicked()
{
    // 重置到当前配置
    updateUIFromConfig(m_config);
    ui->saveDir->setText(m_savePath);
    m_tempSavePath = m_savePath;
}

void ServiceSettingWidget::on_changeDirBtn_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "选择保存目录", QCoreApplication::applicationDirPath());
    if (!dir.isEmpty()) {
        m_tempSavePath = dir;
        ui->saveDir->setText(dir);
    }
}

sAesItf ServiceSettingWidget::getAesConfig() const
{
    return m_config;
}

QString ServiceSettingWidget::getSavePath() const
{
    return m_savePath;
}

void ServiceSettingWidget::loadSettings(QSettings &settings)
{
    m_savePath = settings.value("saveDir", "").toString();
    m_tempSavePath = m_savePath;
    ui->saveDir->setText(m_savePath);

    settings.beginGroup("sAesItf");
    m_config.level = static_cast<QAESEncryption::Aes>(settings.value("level", static_cast<int>(m_config.level)).toInt());
    m_config.mode = static_cast<QAESEncryption::Mode>(settings.value("mode", static_cast<int>(m_config.mode)).toInt());
    m_config.padding = static_cast<QAESEncryption::Padding>(settings.value("padding", static_cast<int>(m_config.padding)).toInt());
    m_config.key = QByteArray::fromHex(settings.value("key", QString(m_config.key.toHex())).toString().toUtf8());
    m_config.iv = QByteArray::fromHex(settings.value("iv", QString(m_config.iv.toHex())).toString().toUtf8());
    settings.endGroup();

    // 同步临时配置
    m_tempConfig = m_config;

    // 更新界面
    updateUIFromConfig(m_config);

        // 如果配置成功（即key和iv不为空），就把它们隐藏
        ui->lineEditKey->setText("**********");
        ui->lineEditIv->setText("**********");

}

void ServiceSettingWidget::saveSettings(QSettings &settings)
{
    settings.setValue("saveDir", m_savePath);

    settings.beginGroup("sAesItf");
    settings.setValue("level", static_cast<int>(m_config.level));
    settings.setValue("mode", static_cast<int>(m_config.mode));
    settings.setValue("padding", static_cast<int>(m_config.padding));
    settings.setValue("key", QString(m_config.key.toHex()));
    settings.setValue("iv", QString(m_config.iv.toHex()));
    settings.endGroup();
   // qDebug()<<m_config.key.toHex();
}


