#ifndef MAPPROCESSOR_H
#define MAPPROCESSOR_H
#include "bus_tojson.h"
#include <QThread>
#include <QObject>
#include <QTimer>
#include "DbWriteThread.h"
class MapProcessor: public QThread
{
    Q_OBJECT
public:
    explicit MapProcessor(QObject* parent = nullptr);
    MapProcessor();
    ~MapProcessor();
    void run() override;
    void setBoxList(BoxData& box);
    void setBoxInc(BoxData& ,const QString&);

public slots:
    void PRun(bool flag);
    void SaveTimerTimeout();

signals:
    void checkSend(int,int,int);

private:
    bus_toJson* mBus;
    bool m_running;
    QTimer *saveTimer;
    bool isSaving = false;
    DbWriteThread* dbWriteThread = nullptr;
};
#endif // MAPPROCESSOR_H
