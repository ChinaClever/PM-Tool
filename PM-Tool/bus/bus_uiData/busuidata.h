#ifndef BUSUIDATA_H
#define BUSUIDATA_H
#include "box_uiData/box_ui.h"
#include "data_struct/bus_tojson.h"
#include <QWidget>
#include <QTimer>

namespace Ui {
class busUiData;
}

class busUiData : public QWidget
{
    Q_OBJECT

public:
    explicit busUiData(QWidget *parent = nullptr);
    ~busUiData();


    void udpSend(QJsonObject& );
    void init();//公共数据初始化
    void createJsonData();
    void conSlots();
    EnvItem temInit();
    Info infoInti();


    void saveSettings(QSettings &settings);
    void loadSettings(QSettings &settings);
    void hideEvent(QHideEvent *event) override;

public slots:

    void createBox(int cnt);
    void on_cirNum_activated(int index);
    void on_outNum_valueChanged(QString);

    void teminit();


    void renew();
    void on_sendJsonBtn_clicked();
    void renewui();
    int curCap();

    BusData setBusTotal(BusData &);


private:
    int busid = 1;
    Ui::busUiData *ui;
    QVector<box_ui*>box;
    QTimer *timer;
    box_ui *box1;
    bus_toJson *work;
};

#endif // BUSUIDATA_H
