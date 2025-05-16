#ifndef TRIPHASEJSONQUEUE_H
#define TRIPHASEJSONQUEUE_H

#include <QObject>
#include <QThread>
class TriPhaseJsonQueue:public QThread
{
public:
    explicit TriPhaseJsonQueue(QObject* parent = nullptr);
    TriPhaseJsonQueue();
};

#endif // TRIPHASEJSONQUEUE_H
