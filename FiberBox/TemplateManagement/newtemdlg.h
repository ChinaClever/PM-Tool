#ifndef NEWTEMDLG_H
#define NEWTEMDLG_H

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

private slots:
    void on_cancelBtn_clicked();

    void on_saveBtn_clicked();

    void on_FiberCount_currentIndexChanged(int index);

    void on_FanoutCount_currentIndexChanged(int index);

    void on_Interface_currentIndexChanged(int index);

    void on_Polarity_currentIndexChanged(int index);

    void on_FiberSpec_currentIndexChanged(int index);

private:
    Ui::NewTemDlg *ui;
    QString desc = "";
};

#endif // NEWTEMDLG_H
