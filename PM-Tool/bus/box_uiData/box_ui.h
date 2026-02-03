#ifndef BOX_UI_H
#define BOX_UI_H

#include <QWidget>
#include "bus/data_struct/BusData.h"
#include "SettingsHelper.h"
namespace Ui {
class box_ui;
}

class box_ui : public QWidget
{
    Q_OBJECT

public:
    explicit box_ui(QWidget *parent = nullptr);
    ~box_ui();
    void setid(int x);
    void timerStart(bool flag);
    void setBoxPhaseData();
    int getOutletGroup(int r, int rowCount, int outletPhase);
    BoxData generaData();
    BusData generaBus();

    QTimer* timer;



public slots:
    void RowEdit(int row);
    void setEle();
    void CntChanged(int cnt);
    void setcirPower(const int row);

private:
    Ui::box_ui *ui;
    int id = 0;
    BoxData data;
    BusData busdata;
};

#endif // BOX_UI_H
