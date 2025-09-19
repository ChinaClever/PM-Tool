#ifndef FORRM_H
#define FORRM_H

#include <QWidget>

namespace Ui {
class Forrm;
}

class Forrm : public QWidget
{
    Q_OBJECT

public:
    explicit Forrm(QWidget *parent = nullptr);
    ~Forrm();

private:
    Ui::Forrm *ui;
};

#endif // FORRM_H
