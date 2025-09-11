#ifndef SETTINGSHELPER_H
#define SETTINGSHELPER_H

#include <QSettings>
#include <QSpinBox>
#include <QDoubleSpinBox>  // 新增
#include <QLineEdit>
#include <QCheckBox>
#include "aesitf.h"
namespace SettingsHelper {

    // 保存 sAesItf 到 QSettings
    inline void saveAesItf(QSettings &s, const sAesItf &aes)
    {
        s.beginGroup("sAesItf");
        s.setValue("level", static_cast<int>(aes.level));
        s.setValue("mode", static_cast<int>(aes.mode));
        s.setValue("padding", static_cast<int>(aes.padding));
        s.setValue("key", QString(aes.key.toHex()));
        s.setValue("iv", QString(aes.iv.toHex()));
        s.endGroup();
    }

    // 从 QSettings 加载 sAesItf
    inline void loadAesItf(QSettings &s, sAesItf &aes)
    {
        s.beginGroup("sAesItf");
        aes.level = static_cast<QAESEncryption::Aes>(s.value("level", static_cast<int>(aes.level)).toInt());
        aes.mode = static_cast<QAESEncryption::Mode>(s.value("mode", static_cast<int>(aes.mode)).toInt());
        aes.padding = static_cast<QAESEncryption::Padding>(s.value("padding", static_cast<int>(aes.padding)).toInt());
        aes.key = QByteArray::fromHex(s.value("key", QString(aes.key.toHex())).toString().toUtf8());
        aes.iv = QByteArray::fromHex(s.value("iv", QString(aes.iv.toHex())).toString().toUtf8());
        s.endGroup();
    }

} // namespace SettingsHelper

#endif
