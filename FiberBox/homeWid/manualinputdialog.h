// ManualInputDialog.h
#pragma once
#include <QDialog>
#include <QString>
#include <QTableWidgetItem>

namespace Ui {
class ManualInputDialog;
}

class ManualInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ManualInputDialog(QWidget *parent = nullptr);
    ~ManualInputDialog();

    // 设置模板默认信息
    void setTemplateInfo(const QString &fanoutPn, const int &wave, int fiberCount);

    // 获取生成的扫码字符串
    QString getScanCode() const;

private slots:
    void on_spinFiberCount_valueChanged(int count);
    void on_btnOK_clicked();
    void on_btnCancel_clicked();

private:
    Ui::ManualInputDialog *ui;
    QString templatePN;
    QString fanoutPN;
    QString m_scanCode;

    void adjustTableRows(int rowCount);
};
