#include "triphasejsonqueue.h"
#include <QDebug>

TriPhaseJsonQueue::TriPhaseJsonQueue(QObject* parent)
    : QThread(parent)
{
    qDebug() << "Processor created";
}
