#ifndef NAVARWID_H
#define NAVARWID_H

#include <QWidget>

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

    void on_templateBtn_clicked();

    void on_MainWid_clicked();

private:
    Ui::navarwid *ui;
};

#endif // NAVARWID_H
