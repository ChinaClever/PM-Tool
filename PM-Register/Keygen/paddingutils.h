#ifndef PADDINGUTILS_H
#define PADDINGUTILS_H

#include <QByteArray>

namespace PaddingUtils {

QByteArray removePKCS7Padding(const QByteArray &data);
QByteArray removeISO7816Padding(const QByteArray &data);
QByteArray removeZeroPadding(const QByteArray &data);
QByteArray removeISO10126Padding(const QByteArray &data);

}

#endif // PADDINGUTILS_H
