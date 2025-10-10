#ifndef FIBERTAB_H
#define FIBERTAB_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QGroupBox>
#include <QGridLayout>
#include <QTabWidget>
#include <QRegularExpression>
#include <QHeaderView>

class FiberTab : public QWidget
{
    Q_OBJECT
public:
    explicit FiberTab(QWidget *parent = nullptr);

    QTableWidget* getTableWidget() const { return tableWidget; }

    void setInfo(const QString &id,int fiberCount,
                const QString &wavelength,const QString &ilLimit);

    void fillFiberTable(const QString &rawData);

private:
    QTableWidget *tableWidget;

    QLabel *labelId;
    QLabel *labelFiber;
    QLabel *labelWl;
    QLabel *labelIL;

    QLineEdit *lineId;
    QLineEdit *lineFiber;
    QLineEdit *lineWl;
    QLineEdit *lineIL;
};

#endif // FIBERTAB_H
