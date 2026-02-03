#ifndef SERIPPORT_H
#define SERIPPORT_H

#include "mp_bulksend.h"
#include "./ip/ip_BulkSend/ip_bulksend.h"
#include "bus_bulksend/busbulk.h"

#include <QWidget>

namespace Ui {
class serIpPort;
}

class serIpPort : public QWidget
{
    Q_OBJECT

public:
    explicit serIpPort(QWidget *parent = nullptr);
    ~serIpPort();

    void setSubModules(ip_BulkSend* ip, mp_bulksend* mp, busBulk* bus);

signals:
    void sendStatusChanged(bool status); // 1 表示开始，0 表示停止
private slots:
    void on_sendAllBtn_clicked();

    void on_radioButton_clicked();

    void on_radioButton_clicked(bool checked);

private:
    Ui::serIpPort *ui;
    busBulk* mBusBulk = nullptr;
    mp_bulksend* mMpBulkSend = nullptr;
    ip_BulkSend* mIpBulkSend = nullptr;

};

#endif // SERIPPORT_H
