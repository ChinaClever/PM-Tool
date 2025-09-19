#include "templateselect.h"
#include "ui_templateselect.h"

TemplateSelect::TemplateSelect(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TemplateSelect)
{
    ui->setupUi(this);

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
        {"260021", "这是模板A的描述信息，可能比较长"},
        {"260024", "模板B描述"},
        {"260023", "模板C描述内容比较长，需要显示完整"},
        {"260034", "模板D描述"},
        {"260033", "模板E描述"}
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
}

TemplateSelect::~TemplateSelect()
{
    delete ui;
}
