#include "tembtnbar.h"
#include "newtemdlg.h"
TemBtnBar::TemBtnBar(QWidget *parent) : SqlBtnBar(parent)
{
    clearHidden();
    queryHidden();
    importHidden();
}

void TemBtnBar::addBtn()
{
    NewTemDlg dlg(this);
    dlg.exec();
}


void TemBtnBar::modifyBtn(int id)
{

}

bool TemBtnBar::delBtn(int id)
{

}
