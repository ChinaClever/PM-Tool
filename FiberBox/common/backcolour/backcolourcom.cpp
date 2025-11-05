#include "backcolourcom.h"

/* 窗口背景色 */
#define WIDGET_BACK_COLOR   232,249,255
#define BUTTON_BACK_COLOR  232,238,250

#define DILAG_BACK_COLOR      85,152,215
#define DILAG_BUTTON_COLOR  255,255,255

/* 获得焦点BTN背景颜色 */
#define BTN_FOCUS_BACK_COLOR    "QPushButton:focus{background-color: rgb(96,238,250);}"

QColor cm_color(int index)
{
    static QColor colorArray[] =
        {QColor(52,233,0),QColor(220,223,0),QColor(255,162,0),QColor(0,147,138),QColor(0,240,226),
         QColor(0,158,240),QColor(0,96,145),QColor(203,161,255),QColor(119,80,168),QColor(248,127,136),
         QColor(169,65,72),QColor(138,196,139),QColor(81,120,82)};
    index = index % (sizeof(colorArray)/sizeof(colorArray[0]));
    return colorArray[index];
}

QColor cm_temp2Color(int value)
{
    if(value>35) return QColor(255,90,0,255);
    if(value>30) return QColor(255,255,0,255);
    if(value>25) return QColor(100,200,105,255);
    if(value>22) return QColor(100,128,255,255);
    if(value>0) return QColor(0,128,255,255);
    return QColor(200,200,200,255);
}

/* 设置背景颜色，兼容 Qt5/Qt6 */
void set_background_color(QWidget *widget, const QColor &color)
{
    QPalette palette = widget->palette();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    palette.setColor(QPalette::Window, color);
#else
    palette.setColor(QPalette::Background, color);
#endif
    widget->setAutoFillBackground(true);
    widget->setPalette(palette);
}
void set_background_icon(QWidget *widget, const QString &icon)
{
    set_background_icon(widget, icon, QSize());  // 调用重载函数，传入默认 QSize()
}
/* 设置背景图片，兼容 Qt5/Qt6 */
void set_background_icon(QWidget *widget, const QString &icon, const QSize &size)
{
    QPalette palette;
    QPixmap map = QPixmap(icon);
    if (size.isValid())
        map = map.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    palette.setBrush(QPalette::Window, QBrush(map));
#else
    palette.setBrush(QPalette::Background, QBrush(map));
#endif

    widget->setAutoFillBackground(true);
    widget->setPalette(palette);
}

/* 设置窗口背景颜色和按钮颜色，保持原有颜色 */
void com_setBackColour(const QString &str, QWidget *target)
{
    target->setWindowTitle(str);
    target->setAutoFillBackground(true);

    QPalette palette = target->palette();
    palette.setBrush(QPalette::Window, QColor(WIDGET_BACK_COLOR));
    palette.setBrush(QPalette::Base, QColor(WIDGET_BACK_COLOR));
    palette.setBrush(QPalette::Button, QColor(BUTTON_BACK_COLOR));
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    palette.setBrush(QPalette::Background, QColor(WIDGET_BACK_COLOR)); // Qt5
#endif

    target->setPalette(palette);
    target->setWindowIcon(QIcon(":/image/logo.jpg"));
}

/* 按钮样式 */
void button_style_sheet(QWidget *target)
{
    target->setStyleSheet("QPushButton{background-color:rgb(102, 199, 249);}"
                          "QPushButton{font-family:'Microsoft YaHei';font-size:14px;color:rgb(0,0,0);}"
                          "QPushButton:hover{background-color:rgb(91,237,238);}"
                          "QPushButton:pressed{background-color:rgb(143,185,181);}"
                          "QComboBox{background-color:rgb(255,255,255);}");
    target->setFont(QFont("微软雅黑",12));
}

void btnBlue_style_sheet(QWidget *target)
{
    target->setStyleSheet("QPushButton{background-color:rgb(12,62,180); color:rgb(255,255,255);}"
                          "QPushButton:hover{background-color:rgb(91,237,238);color:rgb(0,0,0);}"
                          "QPushButton:pressed{background-color:rgb(143,185,181);color:rgb(0,0,0);}");
    target->setFont(QFont("微软雅黑",12));
}

/* 设置 groupBox 背景图片 */
void groupBox_background_icon(QWidget *target)
{
    set_background_icon(target, ":/image/box_back.jpg");
}
