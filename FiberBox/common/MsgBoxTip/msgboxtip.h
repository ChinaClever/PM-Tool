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

    void sendTip(const QString& text, const QColor& color = Qt::yellow)
    {
        emit tipChanged(text, color);
    }

signals:
    void tipChanged(const QString& text, const QColor& color);

private:
    MsgCenter() {}
};

#endif
