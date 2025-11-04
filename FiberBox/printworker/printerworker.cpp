#include "printerworker.h"
#include "http/JQLibrary/JQNet"
#include "http/httpclient.h"
#include <QDebug>


QString printworker::doprint(sLabelInfo info)
{
    QString str1 = "DATE,DESC,PN,QR";
    info.qr.remove(QRegularExpression("\\s+"));
    QString str2 = info.date + ',' + info.desc + ',' + info.PN + ',' + info.qr;

    qDebug()<<str2;
    return httpPostIni(str1 + "\n" + str2 , "16","80");
}

QString printworker::httpPostIni(const QString& data,const QString ip, const QString& host) {
    // 构造 URL
    QString url = QString("http://%1:%2/Integration/FiberBox16F/Execute").arg("192.168.1." + ip).arg(host);
    qDebug() << "URL:" << url;
    qDebug() << "Data:" << data;
    QString str = "";

    // 使用智能指针管理线程和 HttpClient
    QSharedPointer<QThread> thread(new QThread);
    QSharedPointer<AeaQt::HttpClient> http(new AeaQt::HttpClient);

    QEventLoop loop; // 用于等待异步回调
    http->clearAccessCache();
    http->clearConnectionCache();

    QByteArray json = data.toUtf8();
    http->post(url)
        .header("content-type", "plain")
        .onSuccess([&](QString result) {
            qDebug() << "result" << result;
            str = result;
            loop.quit();
        })
        .onFailed([&](QString error) {
            qDebug() << "error" << error;
            str = error;
            loop.quit();
        })
        .onTimeout([&](QNetworkReply*) {
            qDebug() << "http_post timeout";
            str = "timeout";
            loop.quit();
        })
        .timeout(5000) // 增加超时到 5 秒
        .body(json)
        .exec();

    // 将 http 对象移到线程
    http->moveToThread(thread.data());
    QObject::connect(thread.data(), &QThread::started, []() { /* 确保线程启动 */ });
    QObject::connect(thread.data(), &QThread::finished, &loop, &QEventLoop::quit);

    thread->start();
    loop.exec(); // 等待回调完成

    // 确保线程停止
    if (thread->isRunning()) {
        thread->quit();
        thread->wait(1000); // 等待最多 1 秒
    }

    return str;
}
