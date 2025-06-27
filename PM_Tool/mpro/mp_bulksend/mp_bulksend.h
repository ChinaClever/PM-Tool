#ifndef MP_BULKSEND_H
#define MP_BULKSEND_H

#include "powersystemdata.h"
#include "amapprocessor.h"
#include "amapjsonqueue.h"
#include "bmapprocessor.h"
#include "bmapjsonqueue.h"
#include "cmapprocessor.h"
#include "cmapjsonqueue.h"
#include "dmapprocessor.h"
#include "dmapjsonqueue.h"
#include "SettingsHelper.h"
#include <QWidget>
#include <QVector>
namespace Ui {
class mp_bulksend;
}

class mp_bulksend : public QWidget
{
    Q_OBJECT

public:
    explicit mp_bulksend(QWidget *parent = nullptr);
    ~mp_bulksend();

    void intiMap();
    void dataResize(PowerSystemData &);
    void bulkinti(const int );
    void inti();//初始化设置
    void initdevip();

    void processStart();
    void updateButtonState(bool);

    void setInti(PowerSystemData& );//设置初始化
    void envInti(PowerSystemData& ); //传感器初始化
    void bitInti(PowerSystemData& ); //输出位初始化
    static void cirInti(PowerSystemData& );
    static void lineInti(PowerSystemData& );
    static void totalInti(PowerSystemData& );

    void saveSettings(QSettings &settings);
    void loadSettings(QSettings &settings);

    void triggerToggleSend(bool);

signals:
    void ProcessorRun(bool);

public slots:
    void intchange(); //基础信息更新槽函数


private slots:
    void on_mpSendBtn_clicked();

private:
    Ui::mp_bulksend *ui;

    //线程
    AMapProcessor* mAMapProcessor;
    QVector<AMapJsonQueue*> mAJsonQ;

    BMapProcessor* mBMapProcessor;
    QVector<BMapJsonQueue*> mBJsonQ;

    CMapProcessor* mCMapProcessor;
    QVector<CMapJsonQueue*> mCJsonQ;

    DMapProcessor* mDMapProcessor;
    QVector<DMapJsonQueue*> mDJsonQ;

    int cirNum;
    int bitNum;
    int volCap;

    QVector<QString> devip;
    QVector<QString> key;

    int macNum; //主副机数量
    QVector<int> addr ;  //级联地址
    QVector<int> serNum; //系列设备数量


};

#endif // MP_BULKSEND_H
