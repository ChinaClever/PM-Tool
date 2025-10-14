#ifndef TEMMAINWID_H
#define TEMMAINWID_H

#include "tembtnbar.h"

namespace Ui {
class temMainwid;
}

class temMainwid : public QWidget
{
    Q_OBJECT

public:
    explicit temMainwid(QWidget *parent = nullptr);
    ~temMainwid();

private:
    Ui::temMainwid  *ui;
    SqlTableWid     *mDbTable;
    TemBtnBar       *mBtnBar;
};

#endif // TEMMAINWID_H
