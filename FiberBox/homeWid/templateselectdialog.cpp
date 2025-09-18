#include "templateselectdialog.h"
#include "ui_templateselectdialog.h"
#include "backcolourcom.h"
#include <QTableWidgetItem>

TemplateSelectDialog::TemplateSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TemplateSelectDialog)
{
    ui->setupUi(this);
        set_background_icon(this,":/image/back.jpg");
    ui->tableWidgetTop5->setColumnCount(2);
    QStringList headers;
    headers << "模板号" << "描述";
    ui->tableWidgetTop5->setHorizontalHeaderLabels(headers);

    // 固定模板号列宽，描述列自适应
    ui->tableWidgetTop5->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->tableWidgetTop5->setColumnWidth(0, 100); // 模板号列宽100像素
    ui->tableWidgetTop5->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    // 支持文字换行和工具提示
    ui->tableWidgetTop5->setWordWrap(true);
    ui->tableWidgetTop5->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 模拟数据
    struct TemplateInfo { QString code; QString desc; };
    QList<TemplateInfo> top5 = {
        {"TMP001", "这是模板A的描述信息，可能比较长"},
        {"TMP002", "模板B描述"},
        {"TMP003", "模板C描述内容比较长，需要显示完整"},
        {"TMP004", "模板D描述"},
     //   {"TMP005", "模板E描述"}
    };

    ui->tableWidgetTop5->setRowCount(top5.size());
    for(int i=0; i<top5.size(); ++i){
        QTableWidgetItem *codeItem = new QTableWidgetItem(top5[i].code);
        QTableWidgetItem *descItem = new QTableWidgetItem(top5[i].desc);
        descItem->setToolTip(top5[i].desc); // 鼠标悬停显示完整内容

        ui->tableWidgetTop5->setItem(i, 0, codeItem);
        ui->tableWidgetTop5->setItem(i, 1, descItem);
    }

    // 点击行把模板号填入输入框
    connect(ui->tableWidgetTop5, &QTableWidget::cellClicked, this, [=](int row, int col){
        Q_UNUSED(col);
        ui->lineEdit->setText(ui->tableWidgetTop5->item(row, 0)->text());
    });

    // 确认按钮
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &TemplateSelectDialog::accept);
    // 取消按钮
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &TemplateSelectDialog::reject);
}

TemplateSelectDialog::~TemplateSelectDialog()
{
    delete ui;
}

// 外部获取选择的模板号
QString TemplateSelectDialog::selectedTemplate() const
{
    return ui->lineEdit->text().trimmed();
}
