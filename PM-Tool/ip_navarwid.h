#ifndef IP_NAVARWID_H
#define IP_NAVARWID_H

#include <QWidget>

namespace Ui {
class IP_NavarWid;
}

class IP_NavarWid : public QWidget
{
    Q_OBJECT

public:
    explicit IP_NavarWid(QWidget *parent = nullptr);
    ~IP_NavarWid();

signals:
    void navBarSig(int);

private slots:
    void on_coreBtn_clicked();

    void on_mproBtn_clicked();

    void on_ipBtn_clicked();

    void on_setBtn_clicked();

    void on_generBtn_clicked();

private:
    Ui::IP_NavarWid *ui;
};

#endif // IP_NAVARWID_H
