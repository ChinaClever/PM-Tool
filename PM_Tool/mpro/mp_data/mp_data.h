#ifndef MP_DATA_H
#define MP_DATA_H
#include "SettingsHelper.h"
#include <QWidget>
#include <QButtonGroup>
#include <QSpinBox>
#include <QCheckBox>
#include "mp_toJson.h"
#include <QUdpSocket>
namespace Ui {
class mp_data;
}

class mp_data : public QWidget
{
    Q_OBJECT

public:
    explicit mp_data(QWidget *parent = nullptr);
    ~mp_data();



    void inti();//公共数据初始化
    void Ddatainti(); //D系列初始化
    void Adatainti(); //A系列初始化

    void setBitcur();
    void ranPf(const int row);
    void setBitPower(const int row);//

    void setBitEle(const int); //输出位电能

    void BitRowEdit(int row); //双击编辑行数据
    void seriesinti();
    void DcuicurData();

    void udpsend();

    void hideEvent(QHideEvent *event);
    void saveSettings(QSettings &settings);
    void loadSettings(QSettings &settings);

public slots:

    void timesend();
    void sensorchange(); //传感器状态改变函数
    void phaseData();   //更新pdu数据
    void bitCntChanged(); //输出位数量改变函数
    void criCntChanged();   //回路数量改变函数
    void onGlobalTimerTimeout(); //增量更新数据函数
    void cuicurData(int x); //更新回路数据
    void on_sendJsonBtn_clicked();
signals:
    void seriesSelectionChanged(int seriesId); //发送系列改变函数

    void cuicurDatachange(int x);//发送更新回路数据信号

    void toJsonSig(PowerSystemData);

private:

    mp_toJson *mTojson;

    Ui::mp_data *ui;
    QButtonGroup * Group_phase ;
    QButtonGroup * Group_series ;
    QList<QDoubleSpinBox*> temBoxes;
    QList<QDoubleSpinBox*> humBoxes;

    double bitEle[50] = {0};

    QList<QCheckBox*> sensors;

    QTimer* m_DglobalUpdateTimer; //定时器更新电流值
};

#endif // MP_DATA_H
