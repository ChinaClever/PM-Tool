#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ip_navarwid.h"
#include "ip_jsondata.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void intiWid();
protected slots:
    void navBarSlot(int);

private:
    Ui::MainWindow *ui;
    IP_JsonData *mIPJsonData;
    IP_NavarWid *mIPNavarWid;
};
#endif // MAINWINDOW_H
