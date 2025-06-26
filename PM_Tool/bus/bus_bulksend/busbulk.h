#ifndef BUSBULK_H
#define BUSBULK_H

#include "MapJsonQueue/mapjsonqueue.h"
#include "MapProcessor/mapprocessor.h"
#include "BusData.h"
#include "SettingsHelper.h"
#include <QWidget>

namespace Ui {
class busBulk;
}

class busBulk : public QWidget
{
    Q_OBJECT

public:
    explicit busBulk(QWidget *parent = nullptr);
    ~busBulk();
    void updateButtonState(bool); //发送时限制控件状态
    void init();
    void intiMap();
    void processStart();

    void DataResize(Busbar&);
    void setInfo(Info &);
    void envTem(EnvItem& );
    void bulkinti(const int );//第几条母线

    void setBoxCfg(BoxConfig&);
    void setBoxLoop(BoxData& );
    static void setBoxList(BoxData& );

    void setBusCfg(BusConfig&);
    static void setBusline(BusData&);
    static void setBusTotal(BusData&);

    void saveSettings(QSettings &settings);
    void loadSettings(QSettings &settings);

    void triggerToggleSend(bool);

signals:
    void ProcessorRun(bool);
private slots:
    void on_busSendBtn_clicked();

    void on_bus_timeInv_valueChanged(int arg1);

private:

    Ui::busBulk *ui;
    MapProcessor* mMapProcessor;
    QVector<MapJsonQueue*> mJsonQ;

    int busNum;
    int boxNum;
    int volCap;

    QString devip;
    QString key;
    int index;
    QVector<int> addr ;
};

#endif // BUSBULK_H
