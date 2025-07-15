#include "mainwindow.h"

#include <QApplication>
#include <QProcess>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    // 启用退出锁定
    QCoreApplication::setQuitLockEnabled(true);

    // 在应用程序退出时终止后台进程
    QObject::connect(&a, &QCoreApplication::aboutToQuit, []() {
        QProcess::startDetached("taskkill /F /IM PM-Tool.exe");
    });
    return a.exec();
}
