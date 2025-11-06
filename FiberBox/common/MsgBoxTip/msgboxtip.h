#ifndef MSGCENTER_H
#define MSGCENTER_H

#include <QObject>
#include <QString>
#include <QColor>

class MsgCenter : public QObject
{
    Q_OBJECT
public:
    static MsgCenter* instance()
    {
        static MsgCenter _instance;
        return &_instance;
    }

    void sendTip(const QString& text, const QColor& color = Qt::yellow, bool append = false)
    {
        emit tipChanged(text, color, append);
    }

signals:
    //void tipChanged(const QString& text, const QColor& color);                       // 覆盖旧内容
    void tipChanged(const QString& text, const QColor& color, bool append);          // 控制是否追加

private:
    MsgCenter() {}
};

#endif
