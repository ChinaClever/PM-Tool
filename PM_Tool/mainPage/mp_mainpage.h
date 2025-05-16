#ifndef MP_MAINPAGE_H
#define MP_MAINPAGE_H

#include "./ip/ip_BulkSend/ip_bulksend.h"
#include <QWidget>

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
    ip_BulkSend *mIpBulkSend;
};

#endif // MP_MAINPAGE_H
