#ifndef TEMPLATESELECTDIALOG_H
#define TEMPLATESELECTDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class TemplateSelectDialog;
}

class TemplateSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TemplateSelectDialog(QWidget *parent = nullptr);
    ~TemplateSelectDialog();

    // 返回用户选择或输入的模板号
    QString selectedTemplate() const;

private slots:


private:
    Ui::TemplateSelectDialog *ui;
};

#endif // TEMPLATESELECTDIALOG_H
