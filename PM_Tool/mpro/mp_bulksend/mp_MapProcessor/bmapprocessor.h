#ifndef BMAPPROCESSOR_H
#define BMAPPROCESSOR_H

#include <QObject>
#include <QThread>
class BMapProcessor:public QThread
{
public:
    explicit BMapProcessor(QObject* parent = nullptr);
    BMapProcessor();
    ~BMapProcessor();
    void run()override;
public slots:
    void BPRun(bool flag);
private:
    bool m_running;
};

#endif // BMAPPROCESSOR_H
