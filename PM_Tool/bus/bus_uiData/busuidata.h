#ifndef BUSUIDATA_H
#define BUSUIDATA_H
#include "box_uiData/box_ui.h"

#include <QWidget>

namespace Ui {
class busUiData;
}

class busUiData : public QWidget
{
    Q_OBJECT

public:
    explicit busUiData(QWidget *parent = nullptr);
    ~busUiData();


    void init();//公共数据初始化

    void conSlots();

public slots:

    void createBox(int cnt);
    void on_cirNum_activated(int index);
    void on_outNum_valueChanged();

private:
    Ui::busUiData *ui;
    QVector<box_ui*>box;
    box_ui *box1;
};

#endif // BUSUIDATA_H
