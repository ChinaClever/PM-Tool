#ifndef MAINWID_H
#define MAINWID_H
#include <QWidget>
#include <QLineEdit>
#include "fibeltem.h"
#include "fibercheck.h"
#include "msgboxtip.h"
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

protected:
    void    setTemInfo();
    void    setAccessoryVisible(int count);

private slots:
    void    on_ConfirmTpl_clicked();
    void    handleScanCode(const QString &code); // 处理每次扫码

private:
    Ui::mainWid  *ui;
    FiberTem     *mFiberTem;
    MsgCenter    *msgCenter;

    QLineEdit    *scanInput;  // 隐藏扫码输入框

    FiberCheck    checker;
    int           fanIndex = 0;      // 当前扫码序号
    int           totalFans ;
    int           m_currentAccessoryIndex = 0; //待完成 === 显示手动输入按钮 逻辑功能
};

#endif // MAINWID_H
