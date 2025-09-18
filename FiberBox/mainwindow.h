#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "navarwid.h"
#include "homeWid/mainwid.h"
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
    void      init();
public slots:
    void      navBarSlot(int);
private:
    Ui::MainWindow      *ui;
    navarwid            *navigation;
    mainWid             *mainpage;
};
#endif // MAINWINDOW_H
