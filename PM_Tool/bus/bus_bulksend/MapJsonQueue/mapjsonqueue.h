#ifndef MAPJSONQUEUE_H
#define MAPJSONQUEUE_H

#include <QObject>
#include <QThread>

class MapJsonQueue:public QThread
{
    Q_OBJECT
public:
    explicit MapJsonQueue(QObject* parent = nullptr);
    MapJsonQueue();
    ~MapJsonQueue();

    void run() override;
public slots:
    void JRun(bool flag);

private:
    bool m_running;
};
#endif // MAPJSONQUEUE_H
