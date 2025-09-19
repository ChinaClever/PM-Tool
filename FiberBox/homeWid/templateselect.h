#ifndef TEMPLATESELECT_H
#define TEMPLATESELECT_H

#include <QWidget>

namespace Ui {
class TemplateSelect;
}

class TemplateSelect : public QWidget
{
    Q_OBJECT

public:
    explicit TemplateSelect(QWidget *parent = nullptr);
    ~TemplateSelect();

private:
    Ui::TemplateSelect *ui;
};

#endif // TEMPLATESELECT_H
