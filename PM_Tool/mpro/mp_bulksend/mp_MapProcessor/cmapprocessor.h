#ifndef CMAPPROCESSOR_H
#define CMAPPROCESSOR_H

#include <QObject>
#include <QThread>
class CMapProcessor:public QThread
{
public:
    explicit CMapProcessor(QObject* parent = nullptr);
    CMapProcessor();
    ~CMapProcessor();
    void run()override;
public slots:
    void CPRun(bool flag);
private:
    bool m_running;
};

#endif // CMAPPROCESSOR_H
