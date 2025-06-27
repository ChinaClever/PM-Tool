#include "busuidata.h"
#include "ui_busuidata.h"
#include "backcolour/backcolourcom.h"
#include "BusData.h"
#include "bus_globals/bus_globals.h"
#include "specrannumggen.h"

#include <QJsonArray>
#include <QVector>
#include <QDebug>
#include <QUdpSocket>
#include <QThread>
#include <math.h>

busUiData::busUiData(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::busUiData)
    , timer(new QTimer(this))
    , work(new bus_toJson(this))
{
    ui->setupUi(this);
    QString configPath = QCoreApplication::applicationDirPath() + "/config.ini";
    QSettings settings(configPath, QSettings::IniFormat);

    loadSettings(settings);   // 加载配置数据

    init();
    conSlots();
    createBox(9);
    teminit();

}

void busUiData::init()
{
    ui->Table->setStyleSheet("QTabWidget:pane {border-top:0px solid #e8f3f9;background:  transparent; }");
    set_background_icon(this,":/image/box_back.jpg");
    ui->serIp->setValidator(new QRegExpValidator(QRegExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b")));
    ui->devIp->setValidator(new QRegExpValidator(QRegExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b")));

    ui->port->setValidator(new QIntValidator(0, 65532,ui->port));

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
    connect(timer, &QTimer::timeout, this,&busUiData::renew);
}

void busUiData::createBox(int cnt)
{
    boxNum = ui->outNum->value();

    // 先删除多余的box和tab
    while (box.size() > cnt ) {
        int idx = box.size() - 1;
        ui->Table->removeTab(idx+1);

        // 直接delete，避免延迟销毁造成UI不同步
        delete box.last();
        box.removeLast();
    }


    // 获取当前回路数量
    int index = ui->cirNum->currentIndex();
    int cirCnt = (index == 0) ? 3 : (index == 1 ? 6 : 9);

    // 添加新的 box_ui，补足数量
    for (int i = box.size(); i < cnt; ++i) {
        box_ui* box1 = new box_ui(this);

        ui->Table->addTab(box1, QString("插接箱%1").arg(i + 1));
        box1->setid(i);
        box1->CntChanged(cirCnt);

        box.append(box1);
    }


}

void busUiData::on_cirNum_activated(int index)
{
    int cnt = index == 0 ? 3 :(index == 1 ? 6 : 9);

    QFont tableFont;
    cirNum = cnt;
    for(int i = 0; i < ui->boxNum->value(); i++){
        box[i]->CntChanged(cnt);
    }
}

void busUiData::on_outNum_valueChanged(QString)
{
    outletPhase = ui->outNum->value();
}

void busUiData::teminit()
{
    ui->envTem1->setValue(specRanNumGgen::get_temperature());
    ui->envTem2->setValue(specRanNumGgen::get_temperature());
    ui->envTem3->setValue(specRanNumGgen::get_temperature());
    ui->envTem4->setValue(specRanNumGgen::get_temperature());
    ui->box1->setValue(specRanNumGgen::get_temperature());
    ui->box2->setValue(specRanNumGgen::get_temperature());
    ui->box3->setValue(specRanNumGgen::get_temperature());
    ui->box4->setValue(specRanNumGgen::get_temperature());
}

void busUiData::renew()
{
    ui->volA->setValue(ui->phaseV->value());ui->volC->setValue(ui->phaseV->value());ui->volB->setValue(ui->phaseV->value());

    busPhase[0][0] = ui->phaseV->value();
    busPhase[0][1] = ui->phaseV->value();
    busPhase[0][2] = ui->phaseV->value();
    for(int i = 0; i < ui->boxNum->value(); ++i){
        busPhase[1][0] = busPhase[1][1] = busPhase[1][2] = 0;
        busPhase[3][0] = busPhase[3][1] = busPhase[3][2] = 0;
        busPhase[4][0] = busPhase[4][1] = busPhase[4][2] = 0;
        busPhase[5][0] = busPhase[5][1] = busPhase[5][2] = 0;
        busPhase[6][0] = busPhase[6][1] = busPhase[6][2] = 0;
        busPhase[7][0] = busPhase[7][1] = busPhase[7][2] = 0;
    }

    for(int i = 0; i < ui->boxNum->value(); ++i){
        busPhase[1][0] += phase[i][0][0]; //电流
        busPhase[1][1] += phase[i][1][0];
        busPhase[1][2] += phase[i][2][0];

        busPhase[3][0] += phase[i][0][2]; //有功功率
        busPhase[3][1] += phase[i][1][2];
        busPhase[3][2] += phase[i][2][2];

        busPhase[4][0] += phase[i][0][3]; //无功功率
        busPhase[4][1] += phase[i][1][3];
        busPhase[4][2] += phase[i][2][3];

        busPhase[5][0] += phase[i][0][4]; //视在功率
        busPhase[5][1] += phase[i][1][4];
        busPhase[5][2] += phase[i][2][4];

        busPhase[6][0] += phase[i][0][5]; //有功电能
        busPhase[6][1] += phase[i][1][5];
        busPhase[6][2] += phase[i][2][5];

        busPhase[7][0] += phase[i][0][6]; //无功电能
        busPhase[7][1] += phase[i][1][6];
        busPhase[7][2] += phase[i][2][6];

    }

    busPhase[2][0] = busPhase[5][0] == 0 ? 0 : busPhase[3][0] / busPhase[5][0]; //功率因素
    busPhase[2][1] = busPhase[5][1] == 0 ? 0 : busPhase[3][1] / busPhase[5][1];
    busPhase[2][2] = busPhase[5][2] == 0 ? 0 : busPhase[3][2] / busPhase[5][2];

    busPhase[8][0] = specRanNumGgen::get_thd();
    busPhase[8][1] = specRanNumGgen::get_thd();
    busPhase[8][2] = specRanNumGgen::get_thd();

    busPhase[9][0] = busPhase[1][0] / curCap(); //负载率
    busPhase[9][1] = busPhase[1][1] / curCap();
    busPhase[9][2] = busPhase[1][2] / curCap();

    renewui();
}

void busUiData::renewui()
{
    //始端箱相数据赋值
    ui->curA->setValue(busPhase[1][0]);ui->curB->setValue(busPhase[1][1]);ui->curC->setValue(busPhase[1][2]);
    ui->pfA->setValue(busPhase[2][0]);ui->pfB->setValue(busPhase[2][1]);ui->pfC->setValue(busPhase[2][2]);
    ui->act_PowA->setValue(busPhase[3][0]);ui->act_PowB->setValue(busPhase[3][1]);ui->act_PowC->setValue(busPhase[3][2]);
    ui->Rec_PowA->setValue(busPhase[4][0]);ui->Rec_PowB->setValue(busPhase[4][1]);ui->Rec_PowC->setValue(busPhase[4][2]);
    ui->ApparentA->setValue(busPhase[5][0]);ui->ApparentB->setValue(busPhase[5][1]);ui->ApparentC->setValue(busPhase[5][2]);
    ui->eleA->setValue(busPhase[6][0]);ui->eleA->setValue(busPhase[6][1]);ui->eleA->setValue(busPhase[6][2]);
    ui->reacEleA->setValue(busPhase[7][0]);ui->reacEleA->setValue(busPhase[7][1]);ui->reacEleA->setValue(busPhase[7][2]);
    ui->curXbA->setValue(busPhase[8][0]);ui->curXbB->setValue(busPhase[8][1]);ui->curXbC->setValue(busPhase[8][2]);
    ui->loadA->setValue(busPhase[9][0]);ui->loadB->setValue(busPhase[9][1]);ui->loadC->setValue(busPhase[9][2]);

    ui->powAct->setValue(busPhase[3][0]+busPhase[3][2]+busPhase[3][1]);
    ui->powReact->setValue(busPhase[4][0]+busPhase[4][2]+busPhase[4][1]);
    ui->totalPA->setValue(busPhase[5][0]+busPhase[5][2]+busPhase[5][1]);
    ui->totalEleact->setValue(busPhase[6][0]+busPhase[6][2]+busPhase[6][1]);
    ui->totalPf->setValue(ui->totalPA->value() == 0 ? 0 : ui->powAct->value()/ui->totalPA->value());
    ui->volUnbal->setValue(0);
    double cnt = std::max({busPhase[1][0],busPhase[1][1],busPhase[1][2]})
                 - std::min({busPhase[1][0],busPhase[1][1],busPhase[1][2]});
    double avg = (busPhase[1][0]+busPhase[1][1]+busPhase[1][2]) / 3.0;
    ui->curUnbal->setValue(cnt/avg);

    double x =sqrt(busPhase[1][0]*busPhase[1][0]
                    + busPhase[1][1]*busPhase[1][1]
                    + busPhase[1][2]*busPhase[1][2] - busPhase[1][0]*busPhase[1][1]
                    - busPhase[1][1]*busPhase[1][2] - busPhase[1][2]*busPhase[1][0]);
    //零线电流计算规则
    //sqrt(IA*IA + IB*IB + IC*IC - IA*IB - IB*IC - IC*IA);
    ui->zeroCur->setValue(x);

    busid = ui->busId->text().toInt();
    for(int i = 0; i < ui->sendNum->value(); i++){
        busUiData::createJsonData();
        busid++;
    }

}

void busUiData::createJsonData()
{
    EnvItem d = temInit(); //温度
    Info info = infoInti();//基础信息
    //插接箱
    for(auto &u:box){
        BoxData data = u->generaData();
        data.boxCfg.workMode = info.addr;
        info.status = 1;
        info.boxName = "Box-"+QString::number(info.addr);
        info.boxKey = info.busKey + "-" + QString::number(info.addr);
        Busbar g;
        g.boxData = data;
        g.envItemList = d;
        g.info = info;
        info.addr++;
        g.info.barId = busid;
        QJsonObject json = work->toJson(g,1);
        udpSend(json);
    }
    //发送bus
    BusData data = box[0]->generaBus();
    data.busCfg.curSpecs = curCap();
    Busbar g;
    g.busData = setBusTotal(data);
    g.envItemList = d;
    g.info = info;
    g.info.addr = busid;
    g.info.barId = busid;
    QJsonObject json = work->toJson(g,0);
 //   json["addr"] = busid;
    udpSend(json);

}

BusData busUiData::setBusTotal(BusData &data)
{
    auto &u = data.busTotalData;
    u.powValue = ui->powAct->value();
    u.powMin = 0;
    u.powMax = u.powValue;
    u.powStatus = 0;

    u.powApparent = ui->totalPA->value();
    u.powReactive = ui->powReact->value();
    u.powerFactor = ui->totalPf->value();

    u.eleActive = ui->totalEleact->value();
    u.eleApparent = u.eleActive/u.powerFactor;
    u.eleReactive = sqrt(pow(u.eleApparent,2)-pow(u.eleActive,2));
    u.curZeroValue = ui->zeroCur->value();

    u.curResidualValue = std::max(curCap() - ui->curA->value() - ui->curB->value() - ui->curC->value(),0.0);
    u.curResidualAlarm = 0;
    u.curResidualStatus = 0;

    u.curZeroAlarm = u.curZeroValue;
    u.volUnbalance = ui->volUnbal->value();
    u.curUnbalance = ui->curUnbal->value();
    u.hzMax = 60;
    u.hzMin = 50;
    u.hzValue = 55;
    u.hzStatus = 0;

    return data;
}

void busUiData::udpSend(QJsonObject& json)
{
    std::unique_ptr<QUdpSocket> udpSocket = std::make_unique<QUdpSocket>();

    QString ipAddress = ui->serIp->text();
    QString portStr = ui->port->text();
    quint16 Port = portStr.toUShort();

    QByteArray jsonData = QJsonDocument(json).toJson(QJsonDocument::Compact);
    udpSocket->writeDatagram(jsonData, QHostAddress(ipAddress), Port);
    QThread::msleep(1);

}

int busUiData::curCap()
{
    static const int caps[] = {250, 400, 650, 800};
    int index = ui->comboBox->currentIndex();
    return caps[(index >= 0 && index < 3) ? index : 3];
}

Info busUiData::infoInti()
{
    Info data;

    data.addr = ui->addr->value();
    data.devIp = ui->devIp->text();
    data.datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
   // busid = ui->busId->text().toInt();
    data.barId = ui->busId->text().toInt();

    data.busName = QString("Bus-%1").arg(busid);

    data.boxName = QString("Box-%1").arg(data.addr);
    data.busKey = QString("%1-%2").arg(data.devIp).arg(busid, 0, 10);
    data.boxKey = QString("%1-%2").arg(data.busKey).arg(data.addr);


    return data;
}

EnvItem busUiData::temInit()
{
    EnvItem data;
    data.init(4);
    data.boxtem[0] = ui->box1->value();data.boxtem[1] = ui->box2->value();
    data.boxtem[2] = ui->box3->value();data.boxtem[3] = ui->box4->value();

    data.temValue[0] = ui->envTem1->value();
    data.temValue[1] = ui->envTem2->value();
    data.temValue[2] = ui->envTem3->value();
    data.temValue[3] = ui->envTem4->value();

    return data;
}

void busUiData::on_sendJsonBtn_clicked()
{
    if(ui->sendJsonBtn->text()=="开始发送"){
        ui->sendJsonBtn->setText("停止发送");
        for(auto &u:box){
            u->timerStart(1);
        }
        timer->start(ui->timeInv->value()*1000);
        renew();//实时发送一次避免长时间等待
    }
    else{
        ui->sendJsonBtn->setText("开始发送");
        for(auto &u:box){
            u->timerStart(0);
        }
        timer->stop();
    }
}

void busUiData::saveSettings(QSettings &settings)
{
    settings.beginGroup("BoxCirEle");
    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 9; ++j) {
            QString key = QString("cirEle_%1_%2").arg(i).arg(j);
            settings.setValue(key, cirEle[i][j]);  // 直接保存 double
        }
    }
    settings.endGroup();

    settings.beginGroup("BoxCirReacEle");
    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 9; ++j) {
            QString key = QString("cirReacEle_%1_%2").arg(i).arg(j);
            settings.setValue(key, cirReacEle[i][j]);
        }
    }
    settings.endGroup();

    settings.sync();

  //  qDebug() << "Saved BusCirEle and BusCirReacEle data.";
}

void busUiData::loadSettings(QSettings &settings)
{
  //  qDebug() << "Loading BusCirEle and BusCirReacEle data...";
    settings.beginGroup("BoxCirEle");
    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 9; ++j) {
            QString key = QString("cirEle_%1_%2").arg(i).arg(j);
            cirEle[i][j] = settings.value(key, 0.0).toDouble();
        }
    }
    settings.endGroup();

    settings.beginGroup("BoxCirReacEle");
    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 9; ++j) {
            QString key = QString("cirReacEle_%1_%2").arg(i).arg(j);
            cirReacEle[i][j] = settings.value(key, 0.0).toDouble();
        }
    }
    settings.endGroup();

    //qDebug() << "Loaded BusCirEle and BusCirReacEle data.";
}


void busUiData::hideEvent(QHideEvent *event)
{
    //qDebug() << "box_ui::hideEvent called, saving settings.";

    QString configPath = QCoreApplication::applicationDirPath() + "/config.ini";
    QSettings settings(configPath, QSettings::IniFormat);
    saveSettings(settings);

    QWidget::hideEvent(event);  // 保留父类处理
}
