#include "tembtnbar.h"
#include "newtemdlg.h"
#include "edittemdlg.h"
#include "dbtem.h"
#include "fiberdefs.h"

static bool parseDescriptionAndAssign(sTemItem &item, const QString &desc)
{
    QString d = desc.trimmed().toUpper();
    item.data.description = desc;

    // ---- 芯数解析 ----
    QRegularExpression reCount("(\\d+)\\s*\\*\\s*(\\d+)F|(\\d+)F");
    QRegularExpressionMatch mCount = reCount.match(d);
    int fibers = 0;
    if (mCount.hasMatch()) {
        if (!mCount.captured(1).isEmpty() && !mCount.captured(2).isEmpty())
            fibers = mCount.captured(2).toInt();
        else
            fibers = mCount.captured(3).toInt();
    }

    if (fibers == 8) item.data.info.count = FiberCountType::F8;
    else if (fibers == 12) item.data.info.count = FiberCountType::F12;
    else if (fibers == 16) item.data.info.count = FiberCountType::F16;
    else item.data.info.count = FiberCountType::Unknown;

    // ---- 接口类型解析 ----
    if (d.contains("LC") && d.contains("MTP"))
        item.data.info.iface = InterfaceType::MTP_LC;
    else if (d.contains("SN") && d.contains("MTP"))
        item.data.info.iface = InterfaceType::MTP_SN;
    else if (d.contains("MDC") && d.contains("MTP"))
        item.data.info.iface = InterfaceType::MTP_MDC;
    else
        item.data.info.iface = InterfaceType::Unknown;

    // ---- 光纤规格解析 ----
    if (d.contains("OS2"))
        item.data.info.spec = FiberSpec::OS2;
    else if (d.contains("OM4 HV"))
        item.data.info.spec = FiberSpec::OM4_HV;
    else if (d.contains("OM4 AQUA"))
        item.data.info.spec = FiberSpec::OM4_Aqua;
    else if (d.contains("OM4"))
        item.data.info.spec = FiberSpec::OM4_Aqua;
    else if (d.contains("OM5"))
        item.data.info.spec = FiberSpec::OM5;
    else
        item.data.info.spec = FiberSpec::Unknown;

    // ---- 模式 (λ 决定) ----
    if (item.data.lambda.first == 1310 && item.data.lambda.second == 1550)
        item.data.info.mode = FiberMode::SM;
    else if (item.data.lambda.first == 850 && item.data.lambda.second == 1300)
        item.data.info.mode = FiberMode::MM;
    else
        item.data.info.mode = FiberMode::Unknown;

    return true;
}



TemBtnBar::TemBtnBar(QWidget *parent) : SqlBtnBar(parent)
{
    clearHidden();
    queryHidden();
    //importHidden();
}

void TemBtnBar::addBtn()
{
    NewTemDlg dlg(this);
    dlg.exec();
}


void TemBtnBar::modifyBtn(int id)
{
    EditTemDlg dlg(this);
    dlg.setTemId(id);
    dlg.exec();
}

bool TemBtnBar::importBtn()
{
    QString fileName = QFileDialog::getOpenFileName(this, "选择 CSV 文件", "", "CSV 文件 (*.csv)");
    if (fileName.isEmpty())
        return false;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法打开文件");
        return false;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");

    bool firstLine = true;
    int success = 0;
    int total = 0;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;

        // 跳过表头
        if (firstLine) {
            firstLine = false;
            continue;
        }

        // 按制表符或逗号分割
        QStringList cols = line.split(QRegExp("[,\t]"), Qt::SkipEmptyParts);
        if (cols.size() < 11)
            continue;

        total++;
        sTemItem item;
        item.data.PN = cols[0].trimmed();
        item.data.description = cols[1].trimmed();
        item.data.info.polarity = stringToPolarity(cols[2].trimmed());
        item.data.qrTemplate = cols[3].trimmed();
        item.data.FanoutPn = cols[4].trimmed();
        item.data.FanCount = cols[5].toInt();
        item.data.labelTemplate = cols[6].trimmed();
        item.data.lambda.first = cols[8].toInt();
        item.data.lambda.second = cols[9].toInt();
        item.data.limit = cols[10].toDouble();

        parseDescriptionAndAssign(item, item.data.description);

        if (DbTem::build()->insertItem(item))
            success++;
    }

    file.close();

    QMessageBox::information(nullptr, "导入完成",
                             QString("总行数: %1\n成功导入: %2").arg(total).arg(success));

    return true;
}




bool TemBtnBar::delBtn(int id)
{
    if (id <= 0) {
        MsgBox::warning(this, tr("请选择要删除的模板！"));
        return false;
    }

    auto db = DbTem::build();
    bool ret = db->remove(id);

    return ret;
}
