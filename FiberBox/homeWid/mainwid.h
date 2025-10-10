#ifndef MAINWID_H
#define MAINWID_H
#include <QWidget>
#include "fibeltem.h"
#include "fibercheck.h"
#include "msgboxtip.h"
namespace Ui {
class mainWid;
}

class mainWid : public QWidget
{
    Q_OBJECT

public:
    explicit mainWid(QWidget *parent = nullptr);
    ~mainWid();

    void exmple();
    void getInstCon();
    void clearTemplateFields();
    void createFanTable();

protected:
    void setTemInfo();
    void setAccessoryVisible(int count);

private slots:
    void on_ConfirmTpl_clicked();

private:
    Ui::mainWid *ui;
    FiberTem* mFiberTem;
    FiberCheck checker;
    MsgCenter* msgCenter;
};

#endif // MAINWID_H
