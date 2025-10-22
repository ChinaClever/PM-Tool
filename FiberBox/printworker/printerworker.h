#ifndef PRINTERWORKER_H
#define PRINTERWORKER_H
#include <QObject>
#include <QThread>
struct sLabelInfo{
    QString desc;
    QString date;
    QString PN;
    QString qr;
};

class printworker : public QThread
{
    Q_OBJECT
public:
    explicit printworker(QObject *parent = nullptr) : QThread (parent){}
    QString httpPostIni(const QString& data,const QString ip,const QString& host);
public slots:
    QString doprint(const sLabelInfo);
};

#endif // PRINTERWORKER_H
