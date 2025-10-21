#ifndef TEMBTNBAR_H
#define TEMBTNBAR_H

#include <QObject>
#include "sqltablewid.h"
class TemBtnBar : public SqlBtnBar
{
    Q_OBJECT
public:
    explicit TemBtnBar(QWidget *parent = nullptr);
signals:

protected :
    void addBtn()   override;
    bool delBtn(int id)override;
    void modifyBtn(int id)override;
    bool importBtn() override;

};

#endif // TEMBTNBAR_H
