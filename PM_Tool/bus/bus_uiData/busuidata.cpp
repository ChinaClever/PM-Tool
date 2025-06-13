#include "busuidata.h"
#include "ui_busuidata.h"
#include "backcolour/backcolourcom.h"
#include "bus_globals/bus_globals.h"
#include <QVector>
#include <QDebug>

busUiData::busUiData(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::busUiData)
{
    ui->setupUi(this);
    ui->Table->setStyleSheet("QTabWidget:pane {border-top:0px solid #e8f3f9;background:  transparent; }");
    set_background_icon(this,":/image/box_back.jpg");
    init();
    conSlots();

    box1 = new box_ui(this);
    ui->Table->addTab(box1,"插接箱1");
    box1->setid(0);


    createBox(3);


}

void busUiData::init()
{

}

busUiData::~busUiData()
{
    delete ui;
}

void busUiData::conSlots()
{
    connect(ui->cirCur, QOverload<int>::of(&QSpinBox::valueChanged),
            this, [](int value) {
            cirCur = value;
            qDebug() << "更新全局变量 phaseCur =" << cirCur << "A";
            });     //回路电流改变，更新全局
    connect(ui->boxNum, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &busUiData::createBox);  // 插接箱数量改变
}

void busUiData::createBox(int cnt)
{
    while (box.size() > cnt) {
        box.last()->deleteLater();  // 安排对象稍后删除
        box.removeLast();           // 移除指针
    }

    for (int i = box.size(); i < cnt; ++i) {
        box_ui* box1 = new box_ui(this);

        // 添加 tab
      ui->Table->addTab(box1, QString("插接箱%1").arg(i + 1));

        // 设置 id
        box1->setid(i);
        int index  = ui->cirNum->currentIndex();
        int cnt = index == 0 ? 3 :(index == 1 ? 6 : 9);
        box1->CntChanged(cnt);
        // 加入 QVector 管理
        box.append(box1);
    }


}

void busUiData::on_cirNum_activated(int index)
{
    int cnt = index == 0 ? 3 :(index == 1 ? 6 : 9);

    box1->CntChanged(cnt);
    QFont tableFont;

    for(int i = 0; i < ui->boxNum->value(); i++){

        box[i]->CntChanged(cnt);
    }
}

void busUiData::on_outNum_valueChanged()
{
    outNum = ui->outNum->value();

}


void busUiData::on_outNum_valueChanged(int arg1)
{

}

