#ifndef MAINWID_H
#define MAINWID_H

#include <QWidget>
#include "templateselect.h"
#include "fiberscan.h"
namespace Ui {
class mainWid;
}

class mainWid : public QWidget
{
    Q_OBJECT

public:
    explicit mainWid(QWidget *parent = nullptr);
    ~mainWid();
    void fillFiberTable();
    void hide();
private slots:

    void on_pushButton_2_clicked();

private:
    Ui::mainWid *ui;
    TemplateSelect *temselect;
    FiberScan      *fiberscan;
};

#endif // MAINWID_H
