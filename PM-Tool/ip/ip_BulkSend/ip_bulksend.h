#ifndef IP_BULKSEND_H
#define IP_BULKSEND_H

#include <QWidget>
#include "ip_jsondata.h"
#include "ip_sendJson.h"
#include "ip_datapacket.h"
#include "TMapProcessor.h"
#include "SMapProcessor.h"
#include "triphasejsonqueue.h"
#include "sriphasejsonqueue.h"
#include "DbWriteThread.h"
#include "SettingsHelper.h"
namespace Ui {
class ip_BulkSend;
}

class ip_BulkSend : public QWidget
{
    Q_OBJECT

public:
    explicit ip_BulkSend(QWidget *parent = nullptr);
    ~ip_BulkSend();

    void inti();
    void numChangeconnect();
    void intiMap(const int x);
    void threadConnect();

    void saveSettings(QSettings &settings);
    void loadSettings(QSettings &settings);

    void triggerToggleSend(bool);

signals:
    void tmpchange(bool); //1:启动三相处理两个线程，0：关闭线程
    void smpchange(bool); //单相

private slots:
    void on_bulkSendBtn_clicked();
    void on_SbulkSendBtn_clicked();

    void bulkinti(const int x);

    void STNumchange();

    void on_SdevIp_editingFinished();

    void on_devIp_editingFinished();

    void ScheckTime(int);
    void ScheckSend(int,int,int);

    void TcheckTime(int);
    void TcheckSend(int,int,int);

private:
    int tpe;
    int spe;

    int Tnum;           //主机副机个数
    int Snum;


    QString devip;  //三相起始ip
    QString Sdevip; //单相起始ip

    TMapProcessor* m_tmapProcessor;
    QVector<TriPhaseJsonQueue *> m_triphasejson;

    SMapProcessor *m_smapProcessor;
    QVector<SriPhaseJsonQueue *>m_sriphasejson;


    int addr = -1;  //三相设备级联地址
    int Saddr = -1; //单相设备级联地址

    QString key = "";
    QString Skey = "";

    int Ttime = 10;     //发送时间
    int Stime = 10;

    Ui::ip_BulkSend *ui;
    DbWriteThread *m_dbWriteThread;
};

#endif // IP_BULKSEND_H
