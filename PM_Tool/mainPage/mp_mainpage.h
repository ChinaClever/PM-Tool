#ifndef MP_MAINPAGE_H
#define MP_MAINPAGE_H

#include <QWidget>
#include "seripport.h"

namespace Ui {
class mp_mainPage;
}

class mp_mainPage : public QWidget
{
    Q_OBJECT

public:
    explicit mp_mainPage(QWidget *parent = nullptr);
    ~mp_mainPage();

    busBulk* getBusBulk() const { return mBusBulk; }
    mp_bulksend* getMpBulk() const { return mMpBulkSend; }
    ip_BulkSend* getIpBulk() const { return mIpBulkSend; }

private:
    Ui::mp_mainPage *ui;
    ip_BulkSend *mIpBulkSend; //ip批量处理
    mp_bulksend *mMpBulkSend; //Mpro批量处理
    serIpPort *mserIpPort;    //发送地址
    busBulk   *mBusBulk;      //母线批量处理

};

#endif // MP_MAINPAGE_H
