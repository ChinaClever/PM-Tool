#ifndef NAVARWID_H
#define NAVARWID_H

#include <QWidget>
#include "aesitf.h"
#include "keygen.h"
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

    void aesConfigUpdated(const sAesItf &config);


private slots:
    void on_MainWid_clicked();

   // void on_SerSet_clicked();
    void recvUserNameSlot(QString str);
    void on_setBtn_clicked();

    void on_loginBtn_clicked();

    void on_logBtn_clicked();

private:
    Ui::navarwid *ui;
    Keygen *m_keygenWidget = nullptr;
    UsrLandDlg* mUserLand;
};

#endif // NAVARWID_H
