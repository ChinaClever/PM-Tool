#include "mainwindow.h"

#include <QApplication>
#include <QProcess>

#include <MQTTClient.h>
#include <QDebug>

void testMQTT()
{
    qDebug() << "MQTT test begin";

    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

    const char* address = "tcp://test.mosquitto.org:1883";  // 公共 MQTT 服务器
    const char* clientId = "qt_test_001";

    int rc;

    rc = MQTTClient_create(&client, address, clientId,
                           MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if (rc != MQTTCLIENT_SUCCESS) {
        qDebug() << "create failed =" << rc;
        return;
    }

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    rc = MQTTClient_connect(client, &conn_opts);
    if (rc != MQTTCLIENT_SUCCESS) {
        qDebug() << "connect failed =" << rc;
        return;
    }

    const char* payload = "Hello MQTT from Qt";
    rc = MQTTClient_publish(client, "qt/test",
                            strlen(payload), payload,
                            0, false, NULL);

    qDebug() << "publish rc =" << rc;

    MQTTClient_disconnect(client, 1000);
    MQTTClient_destroy(&client);

    qDebug() << "MQTT test end";
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    testMQTT();
    // 启用退出锁定
    QCoreApplication::setQuitLockEnabled(true);

    // 在应用程序退出时终止后台进程
    QObject::connect(&a, &QCoreApplication::aboutToQuit, []() {
        QProcess::startDetached("taskkill /F /IM PM-Tool.exe");
    });
    return a.exec();
}
