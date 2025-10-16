#include "edittemdlg.h"
#include "msgbox.h"
EditTemDlg::EditTemDlg(QWidget* parent) : NewTemDlg(parent)
{
    editTitle("编辑模板");
}
EditTemDlg::~EditTemDlg()
{

}

void EditTemDlg::setTemId(int id)
{
    mId = id;
    sTemItem item;
    auto db = DbTem::build();
    if(!db->getItemById(id, item)) {
        MsgBox::critical(this, tr("未找到模板数据！"));
        return;
    }

    loadTemInfo(item);
}

bool EditTemDlg::saveTemInfo(sTemItem& tem)
{
    tem.id = mId;
    return DbTem::build()->updateItem(tem);
}
