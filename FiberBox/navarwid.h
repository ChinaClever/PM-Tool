#ifndef NAVARWID_H
#define NAVARWID_H

#include <QWidget>
#include "usrlanddlg.h"
namespace Ui {
class navarwid;
}

class navarwid : public QWidget
{
    Q_OBJECT

public:
    explicit navarwid(QWidget *parent = nullptr);
    ~navarwid();

signals:
    void navBarSig(int);

private slots:
    void on_logBtn_clicked();

    void on_setBtn_clicked();

    void recvUserNameSlot(QString str);

    void on_templateBtn_clicked();

    void on_MainWid_clicked();

    void on_loginBtn_clicked();

private:
    Ui::navarwid    *ui;
    UsrLandDlg      *mUserLand;
};

#endif // NAVARWID_H
