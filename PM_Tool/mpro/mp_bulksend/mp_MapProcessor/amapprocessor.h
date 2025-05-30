#ifndef AMAPPROCESSOR_H
#define AMAPPROCESSOR_H

#include <QThread>
#include <QObject>

class AMapProcessor: public QThread
{
    Q_OBJECT
public:
    explicit AMapProcessor(QObject* parent = nullptr);
    AMapProcessor();
    ~AMapProcessor();
    void run() override;

public slots:
    void APRun(bool flag);

private:
    bool m_running;
};

#endif // AMAPPROCESSOR_H
