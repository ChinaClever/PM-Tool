#ifndef SEND_JSON_H
#define SEND_JSON_H

#include <QWidget>

namespace Ui {
class send_json;
}

class send_json : public QWidget
{
    Q_OBJECT

public:
    explicit send_json(QWidget *parent = nullptr);
    void inti();
    int turn(int x);
    ~send_json();

private slots:


    void on_ST_Switch_Com_currentIndexChanged(int index);

    void on_Submit_Json_Btn_clicked();

    void on_envInsert1_currentIndexChanged(int index);

    void on_envInsert2_currentIndexChanged(int index);

    void on_Reset_Btn_clicked();

    void sendJsonViaUdp(const QJsonObject &json);

private:
    Ui::send_json *ui;
};

#endif // SEND_JSON_H
