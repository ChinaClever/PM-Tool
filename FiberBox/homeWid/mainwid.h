#ifndef MAINWID_H
#define MAINWID_H
#include <QWidget>
#include <QLineEdit>
#include "fiberlogstruct.h"
#include "fibeltem.h"
#include "fibercheck.h"
#include "msgboxtip.h"
#include "ScanInfo.h"
#include "serialmgr.h"
#include "dblogs.h"
#include "printerworker.h"
namespace Ui {
class mainWid;
}

class mainWid : public QWidget
{
    Q_OBJECT

public:
    explicit mainWid(QWidget *parent = nullptr);
    ~mainWid();

    void    exmple();
    void    getInstCon();              //初始化槽函数连接
    void    clearTemplateFields();     //清除显示信息
    void    createFanTable();          //创建扇出线Table
    void    init();
    void    AccessoryScanClear();
    void    stopWork(int type);
    void    startWork(const QString &pn);
    bool    checkFiberInTem(ScanInfo& info);

protected:
    void    setTemInfo();
    void    setAccessoryVisible(int count);
    void    handleManualInput(int index);

    bool    eventFilter(QObject *obj, QEvent *event) override;
    void    showEvent(QShowEvent *event) override;
    void    hideEvent(QHideEvent *event) override;

private slots:
    void    on_ConfirmTpl_clicked();

    void    handleScanCode(const QString &code); // 处理每次扫码

    void    on_btnManualInput1_clicked();

    void    on_btnManualInput2_clicked();

    void    on_btnManualInput3_clicked();

    void    on_btnManualInput4_clicked();
signals:
    void doprint(const sLabelInfo);

private:
    Ui::mainWid  *ui;
    FiberTem     *mFiberTem;
    MsgCenter    *msgCenter;
    SerialMgr    *mgr;
    DbLogs       *dblog;
    printworker  *printThread;
    FiberCheck    checker;
    bool          working = false;       // 按钮的“工作中/待机”状态（开始/停止）
    bool          scanningMode = false;  // 是否允许扫码（即模板已确认）
    bool          activePage = false;    // 当前界面是否可见（用于切页自动暂停）
    int           fanIndex = 0;           // 当前已扫码的索引（从0开始）
    int           totalFans = 0;          // 总 fanout 数量
    sFiberLogItem log;
    QSet<QString> scannedIds;

};

#endif // MAINWID_H
