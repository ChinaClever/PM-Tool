#ifndef IP_JSONDATA_H
#define IP_JSONDATA_H

#include <QWidget>
#include "ui_ip_jsondata.h"
#include <QTimer>
#include <ip_sendJson.h>
#include "SettingsHelper.h"
namespace Ui {
class IP_JsonData;
}

class IP_JsonData : public QWidget
{
    Q_OBJECT

public:
    explicit IP_JsonData(QWidget *parent = nullptr);
    ~IP_JsonData();
    void intiSec();
    void timeinti();

    void saveSettings(QSettings &settings);
    void loadSettings(QSettings &settings);
    void closeEvent(QCloseEvent *event);
    void hideEvent(QHideEvent *event);


signals:
    void phaseChangeSig(int);
    void sendSig(const QJsonObject &json,const QString &serip,const QString &port,const int num);

private slots:
    void ST_Switch_Com_Changed(int index);
    void set_current(int index);
    void set_abled(int id);
    void powerdataCal();
    void incrCal();
    void totalData();
    void alarmturn();

    void on_sendJsonBtn_clicked();
    void timechange();
    void timeoutsend();
    void jsonassignment();

    QJsonObject getenvItemlist();
    QJsonObject getlineItemList();
    QJsonObject getpduTotaldata();
    QJsonObject getpduData(const QJsonObject &envItem,     //pdu整合
                           const QJsonObject &lineItem,
                           const QJsonObject &totalData);
    QString getAlarm();

private:


    bool statusflag = true;

    bool incrEnvInc = true;
    bool incrEnvHum = true;

    bool volIncA = true;
    bool volIncB = true;
    bool volIncC = true;

    bool curIncA = true;
    bool curIncB = true;
    bool curIncC = true;

    bool pfIncA = true;
    bool pfIncB = true;
    bool pfIncC = true;

    bool eleInc = true;


    Ui::IP_JsonData *ui;
    ip_sendJson Json; //定义json对象
    QButtonGroup * BtnGroup_phase ;
    QButtonGroup * BtnGroup_ele ;
    QList<QDoubleSpinBox*> spinBoxes ;
    QTimer *m_timer;
    QTimer *incr_timer;
    QTimer *timesend;
};

#endif // IP_JSONDATA_H
