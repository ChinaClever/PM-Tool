#ifndef DMAPPROCESSOR_H
#define DMAPPROCESSOR_H

#include <QObject>
#include <QThread>
class DMapProcessor:public QThread
{
public:
    explicit DMapProcessor(QObject* parent = nullptr);
    DMapProcessor();
    ~DMapProcessor();
    void run()override;
public slots:
    void DPRun(bool flag);
private:
    bool m_running;
};

#endif // DMAPPROCESSOR_H
