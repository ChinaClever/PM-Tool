#ifndef NEWTEMDLG_H
#define NEWTEMDLG_H

#include "dbtem.h"
#include <QDialog>

namespace Ui {
class NewTemDlg;
}

class NewTemDlg : public QDialog
{
    Q_OBJECT

public:
    explicit NewTemDlg(QWidget *parent = nullptr);
    ~NewTemDlg();
    void updateDescription();
    void editTitle(const QString&);
    void loadTemInfo(sTemItem& tem);
    virtual bool saveTemInfo(sTemItem&);
protected:
    bool inputCheck(void);

private slots:
    void on_cancelBtn_clicked();

    void on_saveBtn_clicked();

    void on_FiberCount_currentIndexChanged(int index);

    void on_FanoutCount_currentIndexChanged(int index);

    void on_Interface_currentIndexChanged(int index);

    void on_Polarity_currentIndexChanged(int index);

    void on_FiberSpec_currentIndexChanged(int index);

    void on_EditPN_editingFinished();

private:
    Ui::NewTemDlg *ui;
    QString desc = "";

};

#endif // NEWTEMDLG_H
