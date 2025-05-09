#ifndef IP_JSONDATA_H
#define IP_JSONDATA_H

#include <QWidget>
#include "ui_ip_jsondata.h"
#include <QTimer>
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

signals:
    void phaseChangeSig(int);

private slots:
    void ST_Switch_Com_Changed(int index);
    void set_current(int index);
    void set_abled(int id);
    void powerdataCal();
    void incrCal();
private:

    bool incrEnvInc = true;
    bool incrEnvHum = true;




    Ui::IP_JsonData *ui;

    QButtonGroup * BtnGroup_phase ;
    QButtonGroup * BtnGroup_ele ;

    QTimer *m_timer;
    QTimer *incr_timer;
};

#endif // IP_JSONDATA_H
