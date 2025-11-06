#include <QApplication>
#include <QStyleFactory>
#include <QPalette>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 强制使用跨平台 Fusion 样式（不随系统变）
    app.setStyle(QStyleFactory::create("Fusion"));

    // === 设置固定浅色调色板 ===
    QPalette lightPalette;

    lightPalette.setColor(QPalette::Window, QColor(245, 245, 245));           // 窗口背景
    lightPalette.setColor(QPalette::WindowText, Qt::black);                  // 窗口文字
    lightPalette.setColor(QPalette::Base, Qt::white);                        // 输入框背景
    lightPalette.setColor(QPalette::AlternateBase, QColor(233, 231, 227));
    lightPalette.setColor(QPalette::ToolTipBase, Qt::white);
    lightPalette.setColor(QPalette::ToolTipText, Qt::black);
    lightPalette.setColor(QPalette::Text, Qt::black);
    lightPalette.setColor(QPalette::Button, QColor(240, 240, 240));
    lightPalette.setColor(QPalette::ButtonText, Qt::black);
    lightPalette.setColor(QPalette::BrightText, Qt::red);
    lightPalette.setColor(QPalette::Link, QColor(0, 122, 204));

    lightPalette.setColor(QPalette::Highlight, QColor(0, 120, 215));
    lightPalette.setColor(QPalette::HighlightedText, Qt::white);

    // 应用浅色调色板
    app.setPalette(lightPalette);

//     // （可选）让样式不再受操作系统主题影响
// #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
//     QGuiApplication::setStyleHints(nullptr);  // 禁止自动暗色检测
// #endif
    Cfg *cfg = Cfg::bulid();
    QTranslator translator;
    QString lang = cfg->getLanguage(); // zh / en
    if(lang == "en") {
        QString qmPath = QApplication::applicationDirPath() + "/translations/FiberBox_en.qm";
        if(translator.load(qmPath)) {
            app.installTranslator(&translator);
        }
    }
    MainWindow w;
    w.show();
    return app.exec();
}
