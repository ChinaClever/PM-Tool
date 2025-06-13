#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ip_navarwid.h"
#include "ip_jsondata.h"
#include "./mainPage/mp_mainPage.h"
#include "mp_data.h"
#include "bus/bus_uiData/busuidata.h"
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
    mp_mainPage *mMainPage;
    mp_data  *mProData;
    busUiData *mBusData;
};
#endif // MAINWINDOW_H
