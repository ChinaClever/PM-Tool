#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "send_json.h"

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

private slots:
    void on_MainWidget_Btn_clicked();

private:
    Ui::MainWindow *ui;
    send_json *sj;
};
#endif // MAINWINDOW_H
