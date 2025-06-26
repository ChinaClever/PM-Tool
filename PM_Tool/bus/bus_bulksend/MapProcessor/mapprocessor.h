#ifndef MAPPROCESSOR_H
#define MAPPROCESSOR_H
#include "bus_tojson.h"
#include <QThread>
#include <QObject>

class MapProcessor: public QThread
{
    Q_OBJECT
public:
    explicit MapProcessor(QObject* parent = nullptr);
    MapProcessor();
    ~MapProcessor();
    void run() override;
    void setBoxList(BoxData& box);
    void setBoxInc(BoxData&);

public slots:
    void PRun(bool flag);

private:
    bus_toJson* mBus;
    bool m_running;
};
#endif // MAPPROCESSOR_H
