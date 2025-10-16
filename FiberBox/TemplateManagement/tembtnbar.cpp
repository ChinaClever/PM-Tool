#include "tembtnbar.h"
#include "newtemdlg.h"
#include "edittemdlg.h"
#include "dbtem.h"
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
    EditTemDlg dlg(this);
    dlg.setTemId(id);
    dlg.exec();
}

bool TemBtnBar::delBtn(int id)
{
    if (id <= 0) {
        MsgBox::warning(this, tr("请选择要删除的模板！"));
        return false;
    }

    auto db = DbTem::build();
    bool ret = db->remove(id);

    return ret;
}
