#ifndef KEYGEN_H
#define KEYGEN_H
#include <QButtonGroup>
#include <QWidget>
#include <QJsonObject>
#include <QByteArray>
#include "aesitf.h"
#include "servicesettingwidget.h"
#include "test_corethread.h"
#include <baseobject.h>
#include <QSettings>
namespace Ui {
class Keygen;
}

class Keygen : public QWidget,public BaseObject
{
    Q_OBJECT

public:
    explicit Keygen(QWidget *parent = nullptr);
    ~Keygen();
    void setAesConfig(const sAesItf &config);
    void setSaveDir(const QString &config);

    void loadSettings(QSettings &settings);
    void init();

private slots:
    void on_generateButton_clicked();
    void on_openFileButton_clicked();
    void onSettingsConfigChanged(const sAesItf &config, const QString &savePath);

    void on_tiralBtn_clicked();

    void on_formBtn_clicked();

    void on_copyBtn_clicked();
protected:
    void setDevlogs();

private:
    void decryptFile();

    Ui::Keygen *ui;

    QJsonObject jsonObj;
    QJsonObject featuresObj;
    sAesItf m_aesItf; //加密配置
    QString m_authFilePath;//授权文件路径
    QString m_saveDir; //许可证保存目录

    QString activation_code; //激活码

    QButtonGroup *buttonGroup;

    ServiceSettingWidget *m_settingsWidget;
    QByteArray aesEncrypt(const QByteArray &plainText); //加密
    QByteArray aesDecrypt(const QByteArray &encryptedData); //解密
    void saveEncryptedFile(const QByteArray &data, const QString &fileName); //保存许可证
    QByteArray loadEncryptedFile();//读取授权文件内容信息

    Test_CoreThread *mCoreThread;
};
#endif // KEYGEN_H
