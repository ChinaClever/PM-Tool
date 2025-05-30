#ifndef SERIPPORT_H
#define SERIPPORT_H

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

private:
    Ui::serIpPort *ui;
};

#endif // SERIPPORT_H
