#ifndef SETUP_MAINWID_H
#define SETUP_MAINWID_H
#include <QWidget>
#include "usermainwid.h"

namespace Ui {
class Setup_MainWid;
}

class Setup_MainWid : public QWidget
{
    Q_OBJECT

public:
    explicit Setup_MainWid(QWidget *parent = nullptr);
    ~Setup_MainWid();

protected:
    void initLogCount();
    void writeLogCount();
    void initPcNum();
    void writePcNum();

private slots:
    void on_pcBtn_clicked();

private:
    Ui::Setup_MainWid *ui;
    UserMainWid *mUserWid;
    //sCfgItem *mItem;
};

#endif // SETUP_MAINWID_H
