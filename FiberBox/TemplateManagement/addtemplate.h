#ifndef ADDTEMPLATE_H
#define ADDTEMPLATE_H

#include <QDialog>

namespace Ui {
class addTemplate;
}

class addTemplate : public QDialog
{
    Q_OBJECT

public:
    explicit addTemplate(QWidget *parent = nullptr);
    ~addTemplate();

private:
    Ui::addTemplate *ui;
};

#endif // ADDTEMPLATE_H
