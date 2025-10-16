#ifndef EDITTEMDLG_H
#define EDITTEMDLG_H
#include "newtemdlg.h"
#include "dbtem.h"
class EditTemDlg : public NewTemDlg
{
    Q_OBJECT
public:
    EditTemDlg(QWidget *parent);
    ~EditTemDlg();

    void setTemId(int id);
protected:
    bool saveTemInfo(sTemItem& Tem);
private:
    int mId;
};

#endif // EDITTEMDLG_H
