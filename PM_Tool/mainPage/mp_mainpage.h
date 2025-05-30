#ifndef MP_MAINPAGE_H
#define MP_MAINPAGE_H

#include "./ip/ip_BulkSend/ip_bulksend.h"
#include <QWidget>
#include "mp_bulksend.h"
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

private:
    Ui::mp_mainPage *ui;
    ip_BulkSend *mIpBulkSend; //ip批量处理
    mp_bulksend *mMpBulkSend; //Mpro批量处理
    serIpPort *mserIpPort;    //发送地址
};

#endif // MP_MAINPAGE_H
