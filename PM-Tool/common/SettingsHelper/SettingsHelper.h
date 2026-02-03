#ifndef SETTINGSHELPER_H
#define SETTINGSHELPER_H

#include <QSettings>
#include <QSpinBox>
#include <QDoubleSpinBox>  // 新增
#include <QLineEdit>
#include <QCheckBox>

namespace SettingsHelper {

// 已有的QSpinBox函数
inline void saveSpinBox(QSettings &s, const QString &key, QSpinBox *box) {
    s.setValue(key, box->value());
}

inline void loadSpinBox(QSettings &s, const QString &key, QSpinBox *box) {
    box->setValue(s.value(key, box->value()).toInt());
}

// 新增QDoubleSpinBox支持
inline void saveDoubleSpinBox(QSettings &s, const QString &key, QDoubleSpinBox *box) {
    s.setValue(key, box->value());
}

inline void loadDoubleSpinBox(QSettings &s, const QString &key, QDoubleSpinBox *box) {
    box->setValue(s.value(key, box->value()).toDouble());
}

// 你已有的其它控件函数
inline void saveLineEdit(QSettings &s, const QString &key, QLineEdit *edit) {
    s.setValue(key, edit->text());
}

inline void loadLineEdit(QSettings &s, const QString &key, QLineEdit *edit) {
    edit->setText(s.value(key, edit->text()).toString());
}

inline void saveCheckBox(QSettings &s, const QString &key, QCheckBox *check) {
    s.setValue(key, check->isChecked());
}

inline void loadCheckBox(QSettings &s, const QString &key, QCheckBox *check) {
    check->setChecked(s.value(key, check->isChecked()).toBool());
}

} // namespace SettingsHelper

#endif
