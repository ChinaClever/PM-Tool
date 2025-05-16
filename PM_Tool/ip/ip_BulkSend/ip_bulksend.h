#ifndef IP_BULKSEND_H
#define IP_BULKSEND_H

#include <QWidget>
#include "ip_jsondata.h"
#include "ip_sendJson.h"
#include "ip_datapacket.h"
#include "TMapProcessor.h"
#include "triphasejsonqueue.h"
namespace Ui {
class ip_BulkSend;
}

// extern QMap<QString,IP_sDataPacket<3>>tMap;
// extern QMap<QString,IP_sDataPacket<1>>sMap;

class ip_BulkSend : public QWidget
{
    Q_OBJECT

public:
    explicit ip_BulkSend(QWidget *parent = nullptr);
    ~ip_BulkSend();

    void inti();
    void numChangeconnect();
    void intiMap(const int x);
    //const QMap<QString, IP_sDataPacket<3>>& getTMap() const { return tMap; }
    //const QMap<QString, IP_sDataPacket<1>>& getSMap() const { return sMap; }
signals:
    void tmpchange(bool);
private slots:
    void on_bulkSendBtn_clicked();
    void on_SbulkSendBtn_clicked();

    void bulkinti(const int x);

    void STNumchange();

private:
    int tpe;
    int spe;

    QString devip;
    QString Sdevip;

    TMapProcessor* m_tmapProcessor;
    TriPhaseJsonQueue *m_triphasejson;

    int addr = -1;
    int Saddr = -1;

    QString key = "";
    QString Skey = "";

    int Ttime = 10;
    int Stime = 10;

    int Tnum;
    int Snum;

    Ui::ip_BulkSend *ui;

};

#endif // IP_BULKSEND_H
