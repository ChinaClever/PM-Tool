#pragma once
#include <QString>
#include <QDate>
#include "config.h"

class SerialMgr
{
public:
    static SerialMgr* instance();

    /**
     * @brief 生成完整唯一ID
     * @param materialCode 物料编码
     */
    QString generateFullCode(const QString &materialCode);

    void saveCurrentNum();  // 保存序号到配置文件
private:
    SerialMgr();

    void checkMonthReset();        // 检测是否进入新月份，按月重置序号
    int loadCurrentNum();          // 从配置文件读取当前序号

private:
    Cfg* m_cfg;
    int m_machineId;
};
