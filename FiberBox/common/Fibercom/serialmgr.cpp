#include "serialmgr.h"
#include <QDebug>

SerialMgr* SerialMgr::instance()
{
    static SerialMgr* instance = nullptr;
    if (!instance)
        instance = new SerialMgr();
    return instance;
}

SerialMgr::SerialMgr()
{
    m_cfg = Cfg::bulid();
    // 从配置文件读取电脑号
    m_machineId = m_cfg->read("pc_num", 1, "Sys").toInt();
    checkMonthReset();
}

// 检测新月份，如果月份变化则重置序号
void SerialMgr::checkMonthReset()
{
    QString lastMonth = m_cfg->read("month", "", "Serial").toString();
    QDate today = QDate::currentDate();
    QString thisMonth = QString("%1-%2")
                            .arg(today.year())
                            .arg(today.month(), 2, 10, QChar('0'));

    if (lastMonth != thisMonth)
    {
        m_cfg->write("month", thisMonth, "Serial");
        m_cfg->write("num", 0, "Serial");
        qDebug() << "新月份，序列号重置:" << thisMonth;
    }
}

// 从配置文件读取当前序号
int SerialMgr::loadCurrentNum()
{
    return m_cfg->read("num", 0, "Serial").toInt();
}

// 保存序号到配置文件
void SerialMgr::saveCurrentNum()
{
    int num = loadCurrentNum();
    num++;
    m_cfg->write("num", num, "Serial");
}

QString SerialMgr::generateFullCode(const QString &materialCode)
{
    checkMonthReset();

    QDate today = QDate::currentDate();
    int yy = today.year() % 100;          // 年份后两位
    int ww = today.weekNumber();          // 周数
    QString yyWww = QString("%1W%2")
                        .arg(yy, 2, 10, QChar('0'))
                        .arg(ww, 2, 10, QChar('0')); // 年周

    int mm = today.month();               // 月份

    // 临时流水号
    int num = loadCurrentNum() + 1;

    // 序列号: 年(2)+月(2)+电脑编号(2)+流水号(4)
    QString serial = QString("%1%2%3%4")
                         .arg(yy, 2, 10, QChar('0'))          // 年
                         .arg(mm, 2, 10, QChar('0'))          // 月
                         .arg(m_machineId, 2, 10, QChar('0')) // 电脑编号
                         .arg(num, 4, 10, QChar('0'));        // 流水号

    // 完整ID: 物料编码 + 年周 + 序列号
    QString fullCode = QString("%1%2%3").arg(materialCode).arg(yyWww).arg(serial);

    return fullCode;
}

