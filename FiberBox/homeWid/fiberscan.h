#ifndef FIBERSCAN_H
#define FIBERSCAN_H

#include <QDialog>

namespace Ui {
class FiberScan;
}

class FiberScan : public QDialog
{
    Q_OBJECT

public:
    explicit FiberScan(QWidget *parent = nullptr);
    ~FiberScan();

private:
    Ui::FiberScan *ui;
};

#endif // FIBERSCAN_H
