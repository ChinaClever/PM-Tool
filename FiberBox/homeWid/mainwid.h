#ifndef MAINWID_H
#define MAINWID_H

#include <QWidget>

namespace Ui {
class mainWid;
}

class mainWid : public QWidget
{
    Q_OBJECT

public:
    explicit mainWid(QWidget *parent = nullptr);
    ~mainWid();

private slots:
    void on_SelectTemplateBtn_clicked();

private:
    Ui::mainWid *ui;
};

#endif // MAINWID_H
