// servicesettingwidget.h
#ifndef SERVICESETTINGWIDGET_H
#define SERVICESETTINGWIDGET_H

#include <QWidget>
#include <QSettings>
#include "aesitf.h"

namespace Ui {
class ServiceSettingWidget;
}

class ServiceSettingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ServiceSettingWidget(QWidget *parent = nullptr);
    ~ServiceSettingWidget();

    sAesItf getAesConfig() const;
    QString getSavePath() const;
    void loadSettings(QSettings &settings);
    void saveSettings(QSettings &settings);

    void setStatus(bool);

signals:
    void configChanged(const sAesItf &config, const QString &savePath);

private slots:
    void on_changeDirBtn_clicked();

    void on_resetBtn_clicked();
    void on_applyBtn_clicked();

private:
    void initDialog();
    void updateUIFromConfig(const sAesItf &config);

    Ui::ServiceSettingWidget *ui;
    sAesItf m_config;        // 当前生效的配置
    sAesItf m_tempConfig;    // 临时配置
    QString m_savePath;      // 当前生效的保存路径
    QString m_tempSavePath;  // 临时保存路径
};

#endif
