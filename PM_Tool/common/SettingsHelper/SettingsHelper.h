#ifndef SETTINGSHELPER_H
#define SETTINGSHELPER_H

#include <QSettings>
#include <QSpinBox>
#include <QLineEdit>
#include <QCheckBox>

namespace SettingsHelper {

inline void saveSpinBox(QSettings &s, const QString &key, QSpinBox *box) {
    s.setValue(key, box->value());
}

inline void loadSpinBox(QSettings &s, const QString &key, QSpinBox *box) {
    box->setValue(s.value(key, box->value()).toInt());
}

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
