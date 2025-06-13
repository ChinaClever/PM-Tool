#ifndef BOX_UI_H
#define BOX_UI_H

#include <QWidget>

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

    void setBoxPhaseData();
    void setBoxoutData();
public slots:
    void RowEdit(int row);
    void setEle();
    void CntChanged(int cnt);
    void setcirPower(const int row);

private:
    QTimer* timer;
    Ui::box_ui *ui;
    int id = 0;

};

#endif // BOX_UI_H
